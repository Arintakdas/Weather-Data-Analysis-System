/**
 * TEST BENCH FOR TEAM LEADER (DSA LOGIC)
 * Purpose: Test Sorting, Searching, and Graph logic in isolation.
 * * Instructions:
 * 1. Compile: gcc test_leader.c -o test_leader
 * 2. Run: ./test_leader
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// --- 1. MOCK HEADER (Simulating weather.h) ---
#define MAX_RECORDS 200
#define CITY_LEN 50
#define DATE_LEN 15
#define MAX_CITIES 5

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"
#define BOLD          "\x1b[1m"

typedef struct {
    int id;
    char city[CITY_LEN];
    float temperature;
    int humidity;
    char date[DATE_LEN];
} WeatherRecord;

// --- 2. MOCK DATA (Simulating main.c) ---
WeatherRecord db[MAX_RECORDS]; // The global database
int count = 0;

const char* CITY_NODES[MAX_CITIES] = {"Mumbai", "Delhi", "Chennai", "Kolkata", "Bangalore"};
int adjMatrix[MAX_CITIES][MAX_CITIES] = {
    {0, 1, 0, 0, 1}, // Mumbai -> Delhi, Bangalore
    {1, 0, 1, 0, 0}, // Delhi -> Mumbai, Chennai
    {0, 1, 0, 1, 0}, // Chennai -> Delhi, Kolkata
    {0, 0, 1, 0, 0}, // Kolkata -> Chennai
    {1, 0, 0, 0, 0}  // Bangalore -> Mumbai
};

// Helper to populate fake data for testing
void setupMockData() {
    count = 0;
    // Add 5 random records
    WeatherRecord data[] = {
        {101, "Mumbai", 35.5, 70, "2023-11-01"},
        {102, "Delhi", 18.2, 40, "2023-11-02"}, // Coldest
        {103, "Chennai", 38.0, 85, "2023-11-03"}, // Hottest
        {104, "Kolkata", 28.5, 60, "2023-11-04"},
        {105, "Bangalore", 22.0, 55, "2023-11-05"}
    };
    
    for(int i=0; i<5; i++) {
        db[count++] = data[i];
    }
    printf(COLOR_YELLOW "[TEST SETUP] Loaded 5 mock records.\n" COLOR_RESET);
}

// =============================================================
// 3. YOUR CODE (The DSA Logic to Test)
// =============================================================

// --- SEARCH ENGINE ---
int caseInsensitiveMatch(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) return 0;
        s1++; s2++;
    }
    return *s1 == *s2;
}

void searchRecords(char *query) {
    printf("\n[SEARCH] Looking for: '%s'\n", query);
    int found = 0;
    for (int i = 0; i < count; i++) {
        if (caseInsensitiveMatch(db[i].city, query) || strcmp(db[i].date, query) == 0) {
            printf(" -> Found: ID %d | %s | %.1f C\n", db[i].id, db[i].city, db[i].temperature);
            found++;
        }
    }
    if (found == 0) printf(COLOR_RED " -> No matches.\n" COLOR_RESET);
}

// --- SORT ENGINE (QuickSort) ---
int partition(WeatherRecord arr[], int low, int high, int key, bool ascending) {
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

// --- ANALYTICS ENGINE (Top 5) ---
void showTop5(bool hottest) {
    // Sort array (descending for hottest, ascending for coldest)
    quickSort(db, 0, count-1, 1, !hottest); 
    
    printf("\n[ANALYTICS] Top %s Days:\n", hottest ? "HOTTEST" : "COLDEST");
    for(int i=0; i<count; i++) {
        printf(" %d. %s : %.1f C\n", i+1, db[i].city, db[i].temperature);
    }
}

// --- GRAPH ENGINE (Adjacency Matrix) ---
void showStormImpactTest(int startCityID) {
    printf("\n[GRAPH] Testing Storm from City ID: %d (%s)\n", startCityID, CITY_NODES[startCityID]);
    
    int risks = 0;
    for(int i=0; i<MAX_CITIES; i++) {
        // Check row 'startCityID' in the matrix
        if(adjMatrix[startCityID][i] == 1) {
            printf(" -> STORM SPREADS TO: %s\n", CITY_NODES[i]);
            risks++;
        }
    }
    if(risks == 0) printf(" -> Storm contained. No neighbors.\n");
}


// =============================================================
// 4. TEST RUNNER (The Temporary Main Function)
// =============================================================
int main() {
    printf(BOLD "=== TEAM LEADER LOGIC TEST BENCH ===\n" COLOR_RESET);
    setupMockData();

    // TEST 1: Searching
    printf("\n" BOLD "--- TEST 1: SEARCH ---" COLOR_RESET);
    searchRecords("Mumbai");   // Should find Mumbai
    searchRecords("London");   // Should fail

    // TEST 2: Sorting (Temperature Ascending)
    printf("\n" BOLD "--- TEST 2: SORT (Low to High) ---" COLOR_RESET "\n");
    quickSort(db, 0, count-1, 1, true);
    for(int i=0; i<count; i++) printf(" %.1f C (%s)\n", db[i].temperature, db[i].city);
    // Verification: Should start with 18.2 (Delhi)

    // TEST 3: Top N (Hottest)
    printf("\n" BOLD "--- TEST 3: TOP N (Hottest) ---" COLOR_RESET);
    showTop5(true);
    // Verification: Should start with 38.0 (Chennai)

    // TEST 4: Graph Theory
    printf("\n" BOLD "--- TEST 4: GRAPH PROPAGATION ---" COLOR_RESET);
    // 0 = Mumbai. Adjacency Matrix says Mumbai connects to Delhi (1) and Bangalore (4)
    showStormImpactTest(0); 

    printf(BOLD "\n[TEST COMPLETE] If outputs match logic, your part is ready.\n" COLOR_RESET);
    return 0;
}