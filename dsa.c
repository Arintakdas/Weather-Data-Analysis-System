#include "weather.h"
#include <ctype.h>

// Helper: Case insensitive string compare
int caseInsensitiveMatch(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
            return 0;
        s1++; s2++;
    }
    return *s1 == *s2;
}

void searchRecords(char *query) {
    printf("\n" BOLD "SEARCH RESULTS: '%s'" COLOR_RESET "\n", query);
    printf("%-5s | %-12s | %-10s | %-12s\n", "ID", "City", "Temp", "Date");
    printf("\n");

    int found = 0;
    for (int i = 0; i < count; i++) {
        // Check City OR Date match
        if (caseInsensitiveMatch(db[i].city, query) || strcmp(db[i].date, query) == 0) {
            printf("%-5d | %-12s | %-10.2f | %-12s\n", 
                   db[i].id, db[i].city, db[i].temperature, db[i].date);
            found++;
        }
    }

    if (found == 0) printf(COLOR_RED "No records found.\n" COLOR_RESET);
}


// Partition: Places pivot in correct sorted position
int partition(WeatherRecord arr[], int low, int high, int key, bool ascending) {
    // Select Pivot (Last Element)
    float pivot = (key == 1) ? arr[high].temperature : (float)arr[high].humidity;
    int i = (low - 1);

    for (int j = low; j < high; j++) {
        float current = (key == 1) ? arr[j].temperature : (float)arr[j].humidity;
        
        // Logical Check: Ascending (<) or Descending (>)
        bool condition = ascending ? (current < pivot) : (current > pivot);

        if (condition) {
            i++;
            // Swap
            WeatherRecord temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
    // Swap pivot to center
    WeatherRecord temp = arr[i + 1];
    arr[i + 1] = arr[high];
    arr[high] = temp;
    return (i + 1);
}

// Recursive QuickSort
void quickSort(WeatherRecord arr[], int low, int high, int key, bool ascending) {
    if (low < high) {
        int pi = partition(arr, low, high, key, ascending);
        quickSort(arr, low, pi - 1, key, ascending);
        quickSort(arr, pi + 1, high, key, ascending);
    }
}

void showTop5(bool hottest) {
    if (count == 0) { printf("No data.\n"); return; }

    // Hottest = Descending (False), Coldest = Ascending (True)
    quickSort(db, 0, count-1, 1, !hottest); 

    char *title = hottest ? "HOTTEST" : "COLDEST";
    char *color = hottest ? COLOR_RED : COLOR_CYAN;

    printf("\n" BOLD "--- TOP 5 %s DAYS ---" COLOR_RESET "\n", title);
    
    int limit = (count < 5) ? count : 5;
    
    for(int i=0; i<limit; i++) {
        printf("%d. %s %-12s : %s%.2f C" COLOR_RESET "\n", 
               i+1, color, db[i].city, color, db[i].temperature);
    }
}


void showStormImpact() {
    printf("\n" BOLD COLOR_YELLOW "STORM WARNING SYSTEM (GRAPH DSA) " COLOR_RESET "\n");
    
    // Display Cities with IDs
    printf("Select Epicenter (Start City):\n");
    for(int i=0; i<MAX_CITIES; i++) printf("%d. %s\n", i+1, CITY_NODES[i]);
    
    int choice;
    printf("Enter City ID: ");
    if(scanf("%d", &choice) != 1) { 
        while(getchar() != '\n'); // Clear buffer on error
        return; 
    }
    choice--; // Adjust for 0-index array

    if(choice < 0 || choice >= MAX_CITIES) {
        printf(COLOR_RED "Invalid City.\n" COLOR_RESET);
        return;
    }

    printf(COLOR_RED "\n[ALERT] Storm detected in %s!\n" COLOR_RESET, CITY_NODES[choice]);
    printf("Checking connected cities in Adjacency Matrix...\n");

    // Check Row in Matrix for connections (1 = Connected)
    int risks = 0;
    for(int i=0; i<MAX_CITIES; i++) {
        if(adjMatrix[choice][i] == 1) {
            printf(" -> WARNING: Storm moving towards " BOLD "%s" COLOR_RESET "\n", CITY_NODES[i]);
            risks++;
        }
    }

    if(risks == 0) printf(" -> No connected cities at risk.\n");
}