#include "weather.h"
#include <time.h> // For random graph generation

// Helper: Case insensitive match
int caseInsensitiveMatch(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) return 0;
        s1++; s2++;
    }
    return *s1 == *s2;
}

// NOTE: trimNewline removed from here because it is already in file_io.c

// --- DYNAMIC GRAPH GENERATION ---
// Scans the loaded data for unique cities and creates random connections
void initGraphFromData() {
    if (count == 0) return;

    if (!is_cli_mode) {
        printf(COLOR_YELLOW "[GRAPH] Extracting cities and simulating network...\n" COLOR_RESET);
    }
    uniqueCityCount = 0;

    for (int i = 0; i < count; i++) {
        bool exists = false;
        // Check if city is already in our list
        for (int j = 0; j < uniqueCityCount; j++) {
            if (caseInsensitiveMatch(CITY_NODES[j], db[i].city)) {
                exists = true;
                break;
            }
        }

        // Add new unique city
        if (!exists && uniqueCityCount < MAX_CITIES) {
            strncpy(CITY_NODES[uniqueCityCount], db[i].city, CITY_LEN);
            uniqueCityCount++;
        }
    }

    // Simulate connections (since CSV has no road data)
    // 15% chance of connection between any two cities
    srand(time(0));
    for(int i = 0; i < uniqueCityCount; i++) {
        for(int j = 0; j < uniqueCityCount; j++) {
            adjMatrix[i][j] = 0;
            if (i != j && (rand() % 100) < 15) {
                adjMatrix[i][j] = 1;
            }
        }
    }
    if (!is_cli_mode) {
        printf(COLOR_GREEN "[GRAPH] Network built with %d Unique Cities.\n" COLOR_RESET, uniqueCityCount);
    }
}

// --- ALGORITHMS ---

void searchRecords(char *query) {
    printf("\n" BOLD "--- SEARCH RESULTS: '%s' ---" COLOR_RESET "\n", query);
    printf("%-5s | %-15s | %-10s | %-12s\n", "ID", "City", "Temp", "Date");
    printf("----------------------------------------------------\n");
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (caseInsensitiveMatch(db[i].city, query) || strcmp(db[i].date, query) == 0) {
            printf("%-5d | %-15s | %-10.2f | %-12s\n",
                   db[i].id, db[i].city, db[i].temperature, db[i].date);
            found++;
        }
    }
    if (found == 0) printf(COLOR_RED "No records found.\n" COLOR_RESET);
}

void binarySearchByDate(char *targetDate) {
    if (count == 0) return;

    // Auto-convert YYYY-MM-DD to DD-MM-YYYY to match CSV format
    char searchDate[DATE_LEN];
    strncpy(searchDate, targetDate, DATE_LEN);

    // Detect YYYY-MM-DD format (length 10, dashes at pos 4 and 7)
    if (strlen(targetDate) == 10 &&
        targetDate[4] == '-' && targetDate[7] == '-') {
        // Convert YYYY-MM-DD -> DD-MM-YYYY
        char yyyy[5], mm[3], dd[3];
        strncpy(yyyy, targetDate,     4); yyyy[4] = '\0';
        strncpy(mm,   targetDate + 5, 2); mm[2]   = '\0';
        strncpy(dd,   targetDate + 8, 2); dd[2]   = '\0';
        snprintf(searchDate, DATE_LEN, "%s-%s-%s", dd, mm, yyyy);
    }

    // Linear scan (CSV dates aren't globally sorted)
    printf("\n" BOLD "--- DATE SEARCH RESULTS: '%s' ---" COLOR_RESET "\n", searchDate);
    printf("Date | City | Avg Temp | 00h | 01h | 02h | 03h | 04h | 05h | 06h | 07h | 08h | 09h | 10h | 11h | 12h | 13h | 14h | 15h | 16h | 17h | 18h | 19h | 20h | 21h | 22h | 23h\n");
    printf("-------------------------------\n");

    int found = 0;
    for (int i = 0; i < count; i++) {
        // Support exact match or partial match (e.g., month-year search)
        if (strcmp(db[i].date, searchDate) == 0 ||
            strstr(db[i].date, searchDate) != NULL) {
            printf("%s | %s | %.2f C", db[i].date, db[i].city, db[i].temperature);
            for(int h=0; h<24; h++) {
                printf(" | %.1f", db[i].hourly_temps[h]);
            }
            printf("\n");
            found++;
        }
    }

    if (found == 0) {
        printf(COLOR_RED "[FAIL] Date '%s' not found.\n" COLOR_RESET, searchDate);
        printf("Tip: Try format DD-MM-YYYY (e.g., 12-12-2025) or YYYY-MM-DD\n");
    } else {
        printf(COLOR_GREEN "\n[OK] Found %d record(s).\n" COLOR_RESET, found);
    }
}


// QuickSort Logic
int partition(WeatherRecord arr[], int low, int high, int key, bool ascending) {
    if (key == 3) {
        char pivot[DATE_LEN];
        strcpy(pivot, arr[high].date);
        int i = (low - 1);
        for (int j = low; j < high; j++) {
            int cmp = strcmp(arr[j].date, pivot);
            bool condition = ascending ? (cmp < 0) : (cmp > 0);
            if (condition) {
                i++;
                WeatherRecord temp = arr[i]; arr[i] = arr[j]; arr[j] = temp;
            }
        }
        WeatherRecord temp = arr[i + 1]; arr[i + 1] = arr[high]; arr[high] = temp;
        return (i + 1);
    }

    float pivot = (key == 1) ? arr[high].temperature : (float)arr[high].humidity;
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        float current = (key == 1) ? arr[j].temperature : (float)arr[j].humidity;
        bool condition = ascending ? (current < pivot) : (current > pivot);
        if (condition) {
            i++;
            WeatherRecord temp = arr[i]; arr[i] = arr[j]; arr[j] = temp;
        }
    }
    WeatherRecord temp = arr[i + 1]; arr[i + 1] = arr[high]; arr[high] = temp;
    return (i + 1);
}

void quickSort(WeatherRecord arr[], int low, int high, int key, bool ascending) {
    if (low < high) {
        int pi = partition(arr, low, high, key, ascending);
        quickSort(arr, low, pi - 1, key, ascending);
        quickSort(arr, pi + 1, high, key, ascending);
    }
}

void showTop5(bool hottest) {
    if (count == 0) return;
    quickSort(db, 0, count-1, 1, !hottest);
    printf("\n" BOLD "--- TOP 5 %s DAYS ---" COLOR_RESET "\n", hottest ? "HOTTEST" : "COLDEST");
    printf("Rank | City | Temperature\n");
    printf("---------------------------\n");
    for(int i=0; i < (count < 5 ? count : 5); i++) {
        printf("%d | %s | %.2f C\n", i+1, db[i].city, db[i].temperature);
    }
}

void predictWeather() {
    printf("\n" BOLD COLOR_YELLOW "--- PREDICTIVE ENGINE (LINEAR REGRESSION) ---" COLOR_RESET "\n");
    if(count < 2) { printf("Not enough data.\n"); return; }

    float sumChange = 0;
    int sample = (count > 100) ? 100 : count; // Analyze last 100 records
    for(int i = count - sample; i < count-1; i++) {
        sumChange += (db[i+1].temperature - db[i].temperature);
    }
    float avgChange = sumChange / (sample-1);
    printf("Based on last %d records, Avg Daily Change: %.2f C\n", sample, avgChange);
    printf("Forecast: " BOLD "%.2f C" COLOR_RESET "\n", db[count-1].temperature + avgChange);
}

// Updated to work with Dynamic Cities
void showStormImpact() {
    printf("\n" BOLD COLOR_YELLOW "--- STORM WARNING SYSTEM (GRAPH DSA) ---" COLOR_RESET "\n");
    printf("Network Size: %d Cities\n", uniqueCityCount);

    char inputCity[50];
    printf("Enter Epicenter City Name: ");
    if (scanf("%s", inputCity) != 1) return;

    int choice = -1;
    for(int i=0; i<uniqueCityCount; i++) {
        if(caseInsensitiveMatch(CITY_NODES[i], inputCity)) {
            choice = i;
            break;
        }
    }

    if(choice == -1) { printf(COLOR_RED "City not found in network.\n" COLOR_RESET); return; }

    printf(COLOR_RED "\n[ALERT] Storm in %s! Spreading to:\n" COLOR_RESET, CITY_NODES[choice]);
    int risks = 0;
    for(int i=0; i<uniqueCityCount; i++) {
        if(adjMatrix[choice][i] == 1) {
            printf(" -> ⚠️  %s\n", CITY_NODES[i]);
            risks++;
        }
    }
    if(risks==0) printf(" -> No connected cities at risk.\n");
}

void showStormImpactForCity(char *inputCity) {
    if (uniqueCityCount == 0) initGraphFromData();
    printf("\n" BOLD COLOR_YELLOW "--- STORM WARNING SYSTEM (GRAPH DSA) ---" COLOR_RESET "\n");
    printf("Network Size: %d Cities\n", uniqueCityCount);

    int choice = -1;
    for(int i=0; i<uniqueCityCount; i++) {
        // Also support index-based if the input is numeric
        if(caseInsensitiveMatch(CITY_NODES[i], inputCity) || atoi(inputCity) == i + 1) {
            choice = i;
            break;
        }
    }

    if(choice == -1) { printf(COLOR_RED "City not found in network.\n" COLOR_RESET); return; }

    printf(COLOR_RED "\n[ALERT] Storm in %s! Spreading to:\n" COLOR_RESET, CITY_NODES[choice]);
    int risks = 0;
    for(int i=0; i<uniqueCityCount; i++) {
        if(adjMatrix[choice][i] == 1) {
            printf(" -> ⚠️  %s\n", CITY_NODES[i]);
            risks++;
        }
    }
    if(risks==0) printf(" -> No connected cities at risk.\n");
}

void generateDashboardJSON() {
    if (count == 0) {
        printf("{}\n");
        return;
    }
    double sum = 0;
    float max_temp = -9999.0;
    char max_city[CITY_LEN] = "";

    float min_temp = 9999.0;
    char min_city[CITY_LEN] = "";

    for (int i = 0; i < count; i++) {
        sum += db[i].temperature;
        if (db[i].temperature > max_temp) {
            max_temp = db[i].temperature;
            strcpy(max_city, db[i].city);
        }
        if (db[i].temperature < min_temp) {
            min_temp = db[i].temperature;
            strcpy(min_city, db[i].city);
        }
    }
    double avg = sum / count;

    printf("{\n");
    printf("  \"total\": %d,\n", count);
    printf("  \"avg_temp\": %.2f,\n", avg);
    printf("  \"hottest\": { \"city\": \"%s\", \"temp\": %.2f },\n", max_city, max_temp);
    printf("  \"coldest\": { \"city\": \"%s\", \"temp\": %.2f }\n", min_city, min_temp);
    printf("}\n");
}

typedef struct {
    char city[CITY_LEN];
    float temp;
} CityExtremes;

void showTop5PerCity(bool hottest) {
    if (count == 0) return;
    
    CityExtremes ext[MAX_CITIES];
    int extCount = 0;
    
    for (int i=0; i<count; i++) {
        int found = -1;
        for (int j=0; j<extCount; j++) {
            if (caseInsensitiveMatch(ext[j].city, db[i].city)) {
                found = j; break;
            }
        }
        if (found == -1 && extCount < MAX_CITIES) {
            strcpy(ext[extCount].city, db[i].city);
            ext[extCount].temp = db[i].temperature;
            extCount++;
        } else if (found != -1) {
            if (hottest && db[i].temperature > ext[found].temp) {
                ext[found].temp = db[i].temperature;
            } else if (!hottest && db[i].temperature < ext[found].temp) {
                ext[found].temp = db[i].temperature;
            }
        }
    }
    
    // Sort ext array
    for (int i=0; i<extCount-1; i++) {
        for (int j=0; j<extCount-i-1; j++) {
            bool swap = hottest ? (ext[j].temp < ext[j+1].temp) : (ext[j].temp > ext[j+1].temp);
            if (swap) {
                CityExtremes t = ext[j]; ext[j] = ext[j+1]; ext[j+1] = t;
            }
        }
    }
    
    printf("\n" BOLD "--- TOP 5 %s CITIES (Aggregate) ---" COLOR_RESET "\n", hottest ? "HOTTEST" : "COLDEST");
    printf("Rank | City | Peak Temp\n");
    printf("---------------------------\n");
    int limit = extCount < 5 ? extCount : 5;
    for(int i=0; i<limit; i++) {
        printf("%d | %s | %.2f C\n", i+1, ext[i].city, ext[i].temp);
    }
}

void showStormImpactForCityJSON(char *inputCity) {
    if (uniqueCityCount == 0) initGraphFromData();

    int choice = -1;
    for(int i=0; i<uniqueCityCount; i++) {
        // Also support index-based if the input is numeric
        if(caseInsensitiveMatch(CITY_NODES[i], inputCity) || atoi(inputCity) == i + 1) {
            choice = i;
            break;
        }
    }

    if(choice == -1) { 
        printf("{ \"error\": \"City not found\" }\n");
        return; 
    }

    printf("{\n");
    printf("  \"epicenter\": \"%s\",\n", CITY_NODES[choice]);
    printf("  \"connected\": [\n");

    int printed = 0;
    for(int i=0; i<uniqueCityCount; i++) {
        if(adjMatrix[choice][i] == 1) {
            if (printed > 0) printf(",\n");
            printf("    \"%s\"", CITY_NODES[i]);
            printed++;
        }
    }
    printf("\n  ]\n");
    printf("}\n");
}

void showTrendJSON(char *city, char *date) {
    if (count == 0) {
        printf("{ \"error\": \"No data to show\" }\n");
        return;
    }
    
    char searchDate[DATE_LEN];
    strncpy(searchDate, date, DATE_LEN);

    // Detect YYYY-MM-DD format and convert to DD-MM-YYYY
    if (strlen(date) == 10 && date[4] == '-' && date[7] == '-') {
        char yyyy[5], mm[3], dd[3];
        strncpy(yyyy, date, 4); yyyy[4] = '\0';
        strncpy(mm, date + 5, 2); mm[2] = '\0';
        strncpy(dd, date + 8, 2); dd[2] = '\0';
        snprintf(searchDate, DATE_LEN, "%s-%s-%s", dd, mm, yyyy);
    }

    int found = -1;
    for (int i = 0; i < count; i++) {
        if (caseInsensitiveMatch(db[i].city, city) && 
            (strcmp(db[i].date, searchDate) == 0 || strstr(db[i].date, searchDate) != NULL)) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("{ \"error\": \"No data found for the given city and date\" }\n");
        return;
    }

    printf("{\n");
    printf("  \"city\": \"%s\",\n", db[found].city);
    printf("  \"date\": \"%s\",\n", db[found].date);
    printf("  \"avg_temp\": %.2f,\n", db[found].temperature);
    printf("  \"hourly\": [\n");
    for (int h = 0; h < 24; h++) {
        printf("    %.1f%s\n", db[found].hourly_temps[h], (h < 23) ? "," : "");
    }
    printf("  ]\n");
    printf("}\n");
}

void showTop5city_records(bool hottest, char *city) {
    if (count == 0) return;
    
    // Create a dynamic array to hold valid records for this city (prevents Stack Overflow)
    WeatherRecord *cityRecs = (WeatherRecord *)malloc(count * sizeof(WeatherRecord));
    if (!cityRecs) {
        printf("Memory allocation failed.\n");
        return;
    }
    
    int c_count = 0;
    
    for (int i=0; i<count; i++) {
        if (caseInsensitiveMatch(db[i].city, city)) {
            cityRecs[c_count++] = db[i];
        }
    }
    
    if (c_count == 0) {
        printf("\n" BOLD "--- TOP 5 %s DAYS IN %s ---" COLOR_RESET "\n", hottest ? "HOTTEST" : "COLDEST", city);
        printf(COLOR_RED "No records found for %s.\n" COLOR_RESET, city);
        free(cityRecs);
        return;
    }
    
    quickSort(cityRecs, 0, c_count-1, 1, !hottest);
    
    printf("\n" BOLD "--- TOP 5 %s DAYS IN %s ---" COLOR_RESET "\n", hottest ? "HOTTEST" : "COLDEST", city);
    printf("Rank | Date | Temperature\n");
    printf("---------------------------\n");
    for(int i=0; i < (c_count < 5 ? c_count : 5); i++) {
        printf("%d | %s | %.2f C\n", i+1, cityRecs[i].date, cityRecs[i].temperature);
    }
    
    free(cityRecs);
}