#include "weather.h"

// --- GLOBAL MEMORY ---
bool is_cli_mode = false;
WeatherRecord db[MAX_RECORDS];
int count = 0;

// Dynamic Graph Memory (Matches weather.h definition)
char CITY_NODES[MAX_CITIES][CITY_LEN];
int uniqueCityCount = 0;
int adjMatrix[MAX_CITIES][MAX_CITIES];

// --- UI HELPERS ---
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pause() {
    printf("\n" BOLD "[PRESS ENTER TO CONTINUE]" COLOR_RESET);
    int c; while ((c = getchar()) != '\n' && c != EOF) { } // Clear buffer
    getchar();
}

void showDashboard() {
    if (count == 0) { printf("No data loaded.\n"); return; }
    printf("\n" BOLD "--- LIVE DASHBOARD (Last 20 Records) ---" COLOR_RESET "\n");
    printf("%-12s | %-20s | %-25s\n", "Date", "City", "Visual Trend");
    printf("----------------------------------------------------------------\n");

    int start = (count > 20) ? count - 20 : 0;
    for (int i = start; i < count; i++) {
        int bars = (int)db[i].temperature / 2;
        printf("%-12s | %-20s | ", db[i].date, db[i].city);
        if (db[i].temperature > 30) printf(COLOR_RED);
        else if (db[i].temperature < 20) printf(COLOR_CYAN);
        else printf(COLOR_GREEN);

        if (bars > 20) bars = 20;
        for(int j=0; j<bars; j++) printf("█");
        printf(COLOR_RESET " %.1f C\n", db[i].temperature);
    }
}

// --- FRONTEND LOGIC (Switch Case) ---
void runFrontend() {
  int choice;
  while (1) {
    clearScreen();
    printf("\n======================================================\n");
    printf(BOLD "       WEATHER ANALYTICS - SYSTEM FRONTEND       " COLOR_RESET "\n");
    printf("======================================================\n");
    printf("1. Reload Data (CSV)\n");
    printf("2. Show Dashboard\n");
    printf("3. Add New Record\n");
    printf("4. Search by City\n");
    printf("5. Search by Date\n");
    printf("6. Top 5 Hottest\n");
    printf("7. Top 5 Coldest\n");
    printf("8. Predict Weather (AI)\n");
    printf("9. Storm Warning (Graph)\n");
    printf("10. Save & Exit\n");
    printf("------------------------------------------------------\n");
    printf("Records: %d | Cities: %d\n", count, uniqueCityCount);
    printf("Enter Choice: ");

    if (scanf("%d", &choice) != 1) {
      while (getchar() != '\n');
      continue;
    }

    switch (choice) {
      case 1:
        loadCSV();
        initGraphFromData(); // Re-generate graph on reload
        pause();
        break;
      case 2: showDashboard(); pause(); break;
      case 3:
          if (count < MAX_RECORDS) {
              WeatherRecord r; r.id = 101 + count;
              printf("Enter City: "); scanf("%s", r.city);
              printf("Enter Temp: "); scanf("%f", &r.temperature);
              printf("Enter Hum: "); scanf("%d", &r.humidity);
              printf("Enter Date: "); scanf("%s", r.date);
              db[count++] = r;
              initGraphFromData(); // Update graph
              printf(COLOR_GREEN "Saved.\n" COLOR_RESET);
          } else printf("Full.\n");
          pause(); break;
      case 4: { char q[50]; printf("City: "); scanf("%s", q); searchRecords(q); pause(); break; }
      case 5: { char d[20]; printf("Date: "); scanf("%s", d); binarySearchByDate(d); pause(); break; }
      case 6: showTop5(true); pause(); break;
      case 7: showTop5(false); pause(); break;
      case 8: predictWeather(); pause(); break;
      case 9: showStormImpact(); pause(); break;
      case 10: saveCSV(); printf("Exiting...\n"); return;
      default: printf("Invalid.\n"); pause(); break;
    }
  }
}

int main(int argc, char *argv[]) {
    // If CLI arguments are provided, bypass menu and respond immediately (Frontend Integration)
    if (argc > 1) {
        is_cli_mode = true;
    }

    // 1. Load Data
    loadCSV();

    // 2. Build Graph from Data
    initGraphFromData();

    if (is_cli_mode) {
        if (strcmp(argv[1], "dashboard") == 0) {
            showDashboard();
        } else if (strcmp(argv[1], "dashboard_json") == 0) {
            generateDashboardJSON();
        } else if (strcmp(argv[1], "search_city") == 0 && argc > 2) {
            searchRecords(argv[2]);
        } else if (strcmp(argv[1], "search_date") == 0 && argc > 2) {
            binarySearchByDate(argv[2]);
        } else if (strcmp(argv[1], "top5") == 0 && argc > 2) {
            showTop5(strcmp(argv[2], "hottest") == 0);
        } else if (strcmp(argv[1], "top5_city_records") == 0 && argc > 3) {
            showTop5city_records(strcmp(argv[2], "hottest") == 0, argv[3]);
        } else if (strcmp(argv[1], "top5percity") == 0 && argc > 2) {
            showTop5PerCity(strcmp(argv[2], "hottest") == 0);
        } else if (strcmp(argv[1], "storm") == 0 && argc > 2) {
            showStormImpactForCity(argv[2]);
        } else if (strcmp(argv[1], "storm_json") == 0 && argc > 2) {
            initGraphFromData();
            showStormImpactForCityJSON(argv[2]);
        } else if (strcmp(argv[1], "trend_json") == 0 && argc > 3) {
            showTrendJSON(argv[2], argv[3]);
        } else if (strcmp(argv[1], "load") == 0) {
            printf("Data Loaded Successfully.\n");
        } else if (strcmp(argv[1], "save") == 0) {
            saveCSV();
            printf("Data Saved Successfully.\n");
        } else {
            printf("[FAIL] Unknown command or missing arguments.\n");
        }
        return 0;
    }

    // 3. Start Interface (Terminal User Mode)
    runFrontend();

    return 0;
}