#include "weather.h"
#include <ctype.h>
#include <windows.h>

int caseInsensitiveMatch(const char *s1, const char *s2) {
  while (*s1 && *s2) {
    if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2))
      return 0;
    s1++;
    s2++;
  }
  return *s1 == *s2;
}

void trimNewline(char *s) { s[strcspn(s, "\r\n")] = '\0'; }

void searchRecords(char *query) {
  printf("\n" BOLD "SEARCH RESULTS: '%s'" COLOR_RESET "\n", query);
  printf("%-5s | %-12s | %-10s | %-12s\n", "ID", "City", "Temp", "Date");
  printf("\n");

  int found = 0;
  for (int i = 0; i < count; i++) {
    if (caseInsensitiveMatch(db[i].city, query) ||
        strcmp(db[i].date, query) == 0) {
      printf("%-5d | %-12s | %-10.2f | %-12s\n", db[i].id, db[i].city,
             db[i].temperature, db[i].date);
      found++;
    }
  }

  if (found == 0)
    printf(COLOR_RED "No records found.\n" COLOR_RESET);
}

int partition(WeatherRecord arr[], int low, int high, int key, bool ascending) {
  float pivot = (key == 1) ? arr[high].temperature : (float)arr[high].humidity;
  int i = (low - 1);

  for (int j = low; j < high; j++) {
    float current = (key == 1) ? arr[j].temperature : (float)arr[j].humidity;

    bool condition = ascending ? (current < pivot) : (current > pivot);

    if (condition) {
      i++;
      WeatherRecord temp = arr[i];
      arr[i] = arr[j];
      arr[j] = temp;
    }
  }
  WeatherRecord temp = arr[i + 1];
  arr[i + 1] = arr[high];
  arr[high] = temp;
  return (i + 1);
}

void quickSort(WeatherRecord arr[], int low, int high, int key,
               bool ascending) {
  if (low < high) {
    int pi = partition(arr, low, high, key, ascending);
    quickSort(arr, low, pi - 1, key, ascending);
    quickSort(arr, pi + 1, high, key, ascending);
  }
}

void showTop5(bool hottest) {
  if (count == 0) {
    printf("No data.\n");
    return;
  }

  quickSort(db, 0, count - 1, 1, !hottest);

  char *title = hottest ? "HOTTEST" : "COLDEST";
  char *color = hottest ? COLOR_RED : COLOR_CYAN;

  printf("\n" BOLD "--- TOP 5 %s DAYS ---" COLOR_RESET "\n", title);

  int limit = (count < 5) ? count : 5;

  for (int i = 0; i < limit; i++) {
    printf("%d. %s %-12s : %s%.2f C" COLOR_RESET "\n", i + 1, color, db[i].city,
           color, db[i].temperature);
  }
}

void showTop5PerCity(bool hottest) {
  if (count == 0) {
    printf("No data.\n");
    return;
  }

  char *title = hottest ? "HOTTEST" : "COLDEST";
  char *color = hottest ? COLOR_RED : COLOR_CYAN;

  printf("\n" BOLD "--- TOP 5 %s DAYS PER CITY ---" COLOR_RESET "\n", title);

  for (int cityIdx = 0; cityIdx < MAX_CITIES; cityIdx++) {
    const char *cityName = CITY_NODES[cityIdx];

    WeatherRecord cityRecords[MAX_RECORDS];
    int cityCount = 0;

    for (int i = 0; i < count; i++) {
      if (caseInsensitiveMatch(db[i].city, cityName)) {
        cityRecords[cityCount++] = db[i];
      }
    }

    if (cityCount == 0)
      continue;

    quickSort(cityRecords, 0, cityCount - 1, 1, !hottest);

    printf("\n" BOLD "=== %s ===" COLOR_RESET "\n", cityName);

    int limit = (cityCount < 5) ? cityCount : 5;
    for (int i = 0; i < limit; i++) {
      printf("  %d. %s%.2f C" COLOR_RESET " on %s\n", i + 1, color,
             cityRecords[i].temperature, cityRecords[i].date);
    }
  }
}

void showStormImpact() {
  printf("\n" BOLD COLOR_YELLOW "STORM WARNING SYSTEM (GRAPH DSA) " COLOR_RESET
         "\n");

  printf("Select Epicenter (Start City):\n");
  for (int i = 0; i < MAX_CITIES; i++)
    printf("%d. %s\n", i + 1, CITY_NODES[i]);

  int choice;
  printf("Enter City ID: ");
  if (scanf("%d", &choice) != 1) {
    while (getchar() != '\n')
      ;
    return;
  }
  choice--;

  if (choice < 0 || choice >= MAX_CITIES) {
    printf(COLOR_RED "Invalid City.\n" COLOR_RESET);
    return;
  }

  printf(COLOR_RED "\n[ALERT] Storm detected in %s!\n" COLOR_RESET,
         CITY_NODES[choice]);
  printf("City ID: %d\n",
         choice + 1);
  Sleep(2000); 

  printf("Checking connected cities in Adjacency Matrix...\n");
  Sleep(2000);

  int risks = 0;
  for (int i = 0; i < MAX_CITIES; i++) {
    if (adjMatrix[choice][i] == 1) {
      printf(" -> WARNING: Storm moving towards " BOLD "%s" COLOR_RESET "\n",
             CITY_NODES[i]);
      risks++;
    }
  }
  Sleep(2000);
  if (risks == 0)
    printf(" -> No connected cities at risk.\n");
}

void showStormImpactAt(int choice) {
  if (choice < 0 || choice >= MAX_CITIES) {
    printf(COLOR_RED "Invalid City Index.\n" COLOR_RESET);
    return;
  }

  printf(COLOR_RED "\n[ALERT] Storm detected in %s!\n" COLOR_RESET,
         CITY_NODES[choice]);
  printf("Checking connected cities in Adjacency Matrix...\n");

  int risks = 0;
  for (int i = 0; i < MAX_CITIES; i++) {
    if (adjMatrix[choice][i] == 1) {
      printf(" -> WARNING: Storm moving towards %s\n", CITY_NODES[i]);
      risks++;
    }
  }

  if (risks == 0)
    printf(" -> No connected cities at risk.\n");
}

void saveCSV() {
  FILE *fp = fopen("weather_data_saved.csv", "w");
  if (fp == NULL) {
    printf(COLOR_RED "[ERROR] Could not open file for writing.\n" COLOR_RESET);
    return;
  }

  fprintf(fp, "ID,City,Temperature,Humidity,Date\n");
  for (int i = 0; i < count; i++) {
    fprintf(fp, "%d,%s,%.2f,%d,%s\n", db[i].id, db[i].city, db[i].temperature,
            db[i].humidity, db[i].date);
  }

  fclose(fp);
  printf(COLOR_GREEN
         "[SUCCESS] Saved %d records to weather_data_saved.csv\n" COLOR_RESET,
         count);
}

void loadCSV() {
  FILE *fp = fopen("weather_data_saved.csv", "r");

  if (!fp) {
    printf(COLOR_YELLOW "[INFO] Database file not found. Starting with 0 "
                        "records.\n" COLOR_RESET);
    return;
  }

  char line[4096];
  int row = 0;
  count = 0;

  printf("[LOADING] Reading dataset... please wait.\n");

  int col_city = -1;
  int col_date = -1;
  int col_temp = -1;
  int col_jum = -1;

  fgets(line, sizeof(line), fp);

  if (line[0] == 'P' && line[1] == 'K') {
    printf(COLOR_RED "\n[ERROR] Invalid File Format Detected!\n" COLOR_RESET);
    printf(COLOR_YELLOW "The file 'weather_data.csv' appears to be an Excel "
                        "(.xlsx) or Zip file.\n");
    printf("The C backend cannot read this format directly.\n\n");
    printf(BOLD "SOLUTION:\n" COLOR_RESET);
    printf("1. Open the file in Excel.\n");
    printf("2. Go to File > Save As.\n");
    printf("3. Select 'CSV (Comma delimited) (*.csv)' format.\n");
    printf("4. Overwrite 'weather_data.csv'.\n\n");
    fclose(fp);
    return;
  }

  if (line[0] == '\xEF' && line[1] == '\xBB' && line[2] == '\xBF') {
    memmove(line, line + 3, strlen(line) - 2);
  }

  char *header_token = strtok(line, ",");
  int h_col = 0;
  while (header_token) {
    trimNewline(header_token);
    if (header_token[0] == '"') {
      header_token++;
      char *q = strrchr(header_token, '"');
      if (q)
        *q = '\0';
    }

    if (caseInsensitiveMatch(header_token, "City") ||
        caseInsensitiveMatch(header_token, "City Name"))
      col_city = h_col;
    else if (caseInsensitiveMatch(header_token, "Date") ||
             caseInsensitiveMatch(header_token, "date_time_local"))
      col_date = h_col;
    else if (caseInsensitiveMatch(header_token, "Temperature") ||
             caseInsensitiveMatch(header_token, "Temp") ||
             strstr(header_token, "TempInC") || strstr(header_token, "Temp12"))
      col_temp = h_col;
    else if (caseInsensitiveMatch(header_token, "Humidity"))
      col_jum = h_col;

    header_token = strtok(NULL, ",");
    h_col++;
  }

  printf("[DEBUG] Found Columns - City: %d, Date: %d, Temp: %d, Hum: %d\n",
         col_city, col_date, col_temp, col_jum);

  if (col_city == -1 || col_date == -1 || col_temp == -1) {
    printf(
        COLOR_RED
        "[ERROR] Could not find required columns in CSV header.\n" COLOR_RESET);
    printf("Header line was: %s\n", line);
    if (col_city == -1)
      col_city = 1;
    if (col_date == -1)
      col_date = 4;
    if (col_temp == -1)
      col_temp = 2;
    if (col_jum == -1)
      col_jum = 3;
    printf("[INFO] Using fallback indices: City=%d, Date=%d, Temp=%d, Hum=%d\n",
           col_city, col_date, col_temp, col_jum);
  }

  while (fgets(line, sizeof(line), fp)) {
    if (count >= MAX_RECORDS)
      break;

    char *token;
    int col = 0;

    char *date_str = NULL;
    char *city_str = NULL;
    char *temp_str = NULL;
    char *hum_str = NULL;

    char lineCopy[4096];
    strcpy(lineCopy, line);

    token = strtok(lineCopy, ",");

    while (token) {
      if (col == col_date)
        date_str = token;
      if (col == col_city)
        city_str = token;
      if (col == col_jum)
        hum_str = token;
      if (col == col_temp)
        temp_str = token;

      token = strtok(NULL, ",");
      col++;
    }

    if (row < 5) {
      printf("Row %d: City='%s', Date='%s', Temp='%s'\n", row,
             city_str ? city_str : "NULL", date_str ? date_str : "NULL",
             temp_str ? temp_str : "NULL");
    }
    row++;

    if (city_str && date_str && temp_str) {
      WeatherRecord r;
      r.id = count + 1;

      trimNewline(city_str);
      if (city_str[0] == '"')
        city_str++;
      char *quote = strrchr(city_str, '"');
      if (quote)
        *quote = '\0';
      strncpy(r.city, city_str, CITY_LEN);

      trimNewline(date_str);
      if (date_str[0] == '"')
        date_str++;
      char *dquote = strrchr(date_str, '"');
      if (dquote)
        *dquote = '\0';
      strncpy(r.date, date_str, DATE_LEN);

      if (temp_str[0] == '"')
        temp_str++;
      char *tquote = strrchr(temp_str, '"');
      if (tquote)
        *tquote = '\0';

      r.temperature = atof(temp_str);

      if (hum_str) {
        if (hum_str[0] == '"')
          hum_str++;
        char *hquote = strrchr(hum_str, '"');
        if (hquote)
          *hquote = '\0';
        r.humidity = atoi(hum_str);
      } else {
        r.humidity = 50;
      }

      db[count++] = r;
    }
  }

  fclose(fp);
  printf(COLOR_GREEN "[SUCCESS] Loaded %d records successfully.\n" COLOR_RESET,
         count);
}

void clearScreen() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void pause() {
  printf("\nPress Enter to continue...");
  while (getchar() != '\n')
    ;
}

void showDashboard() {
  printf("\n" BOLD "-- DASHBOARD SUMMARY --" COLOR_RESET "\n");
  printf("Total Records: %d\n", count);
  if (count > 0) {
    printf("Sample record: %s | %.2f C | %d%% | %s\n", db[0].city,
           db[0].temperature, db[0].humidity, db[0].date);
  }
}

void printDashboardJSON() {
  if (count == 0) {
    printf("{\"total\": 0, \"avg_temp\": 0, \"hottest\": null, \"coldest\": "
           "null}");
    return;
  }

  double total_temp = 0;
  int hottest_idx = 0;
  int coldest_idx = 0;

  for (int i = 0; i < count; i++) {
    total_temp += db[i].temperature;
    if (db[i].temperature > db[hottest_idx].temperature)
      hottest_idx = i;
    if (db[i].temperature < db[coldest_idx].temperature)
      coldest_idx = i;
  }

  double avg_temp = total_temp / count;

  printf("{");
  printf("\"total\": %d, ", count);
  printf("\"avg_temp\": %.2f, ", avg_temp);

  printf(
      "\"hottest\": { \"city\": \"%s\", \"temp\": %.2f, \"date\": \"%s\" }, ",
      db[hottest_idx].city, db[hottest_idx].temperature, db[hottest_idx].date);

  printf("\"coldest\": { \"city\": \"%s\", \"temp\": %.2f, \"date\": \"%s\" }",
         db[coldest_idx].city, db[coldest_idx].temperature,
         db[coldest_idx].date);

  printf("}");
}

void predictWeather() {
  printf("\n[Prediction] This is a simple placeholder prediction.\n");
}

void binarySearchByDate(char *date) {
  printf("\n" BOLD "SEARCH BY DATE: %s" COLOR_RESET "\n", date);
  int found = 0;
  for (int i = 0; i < count; i++) {
    if (strcmp(db[i].date, date) == 0) {
      printf("%d. %s : %.2f C | %d%%\n", db[i].id, db[i].city,
             db[i].temperature, db[i].humidity);
      found++;
    }
  }
  if (!found)
    printf(COLOR_RED "No records found for that date.\n" COLOR_RESET);
}

void runFrontend() {
    int choice;

    while (1) {
        clearScreen();
        printf("\n======================================================\n");
        printf(BOLD "       WEATHER ANALYTICS - SIMPLE FRONTEND       " COLOR_RESET "\n");
        printf("======================================================\n");
        printf("1. Load Data from CSV\n");
        printf("2. Show Dashboard\n");
        printf("3. Add New Record\n");
        printf("4. Search by City\n");
        printf("5. Search by Date\n");
        printf("6. Top 5 Hottest\n");
        printf("7. Top 5 Coldest\n");
        printf("8. Storm Warning (DSA)\n");
        printf("9. Save & Exit\n");
        printf("------------------------------------------------------\n");
        printf("Enter Choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {

        case 1:
            loadCSV();
            pause();
            break;

        case 2:
            showDashboard();
            pause();
            break;

        case 3:
            if (count < MAX_RECORDS) {
                WeatherRecord r;
                r.id = 101 + count;

                printf("Enter City: ");
                scanf("%s", r.city);

                printf("Enter Temp (C): ");
                scanf("%f", &r.temperature);

                printf("Enter Humidity (%%): ");
                scanf("%d", &r.humidity);

                printf("Enter Date (YYYY-MM-DD): ");
                scanf("%s", r.date);

                db[count++] = r;
                printf(COLOR_GREEN "[SUCCESS] Record added." COLOR_RESET "\n");
            } else {
                printf(COLOR_RED "Database Full!" COLOR_RESET "\n");
            }
            pause();
            break;

        case 4: {
            char q[50];
            printf("Enter City Name: ");
            scanf("%s", q);
            searchRecords(q);
            pause();
            break;
        }

        case 5: {
            char d[20];
            printf("Enter Date (YYYY-MM-DD): ");
            scanf("%s", d);
            binarySearchByDate(d);
            pause();
            break;
        }

        case 6:
            showTop5(true);
            pause();
            break;

        case 7:
            showTop5(false);
            pause();
            break;

        case 8:
            showStormImpact();
            pause();
            break;

        case 9:
            saveCSV();
            printf(COLOR_YELLOW "Exiting...\n" COLOR_RESET);
            return;

        default:
            printf(COLOR_RED "Invalid Choice." COLOR_RESET "\n");
            pause();
        }
    }
}
