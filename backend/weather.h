#ifndef WEATHER_H
#define WEATHER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- CONSTANTS ---
#define MAX_RECORDS 79000 // Full dataset size
#define CITY_LEN 100      // Length of city names
#define DATE_LEN 15
#define MAX_CITIES 300    // Max unique cities to track in Graph

// --- COLORS ---
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"
#define BOLD          "\x1b[1m"

// --- DATA STRUCTURE ---
typedef struct {
  int id;
  char city[CITY_LEN];
  float temperature;
  int humidity;
  char date[DATE_LEN];
  float hourly_temps[24];
} WeatherRecord;

// --- SHARED GLOBALS ---
extern WeatherRecord db[MAX_RECORDS];
extern int count;
extern bool is_cli_mode;

// Dynamic Graph Globals (Changed to 2D array for dynamic loading)
extern char CITY_NODES[MAX_CITIES][CITY_LEN];
extern int uniqueCityCount;
extern int adjMatrix[MAX_CITIES][MAX_CITIES];

// --- FUNCTION PROTOTYPES ---

// file_io.c
void loadCSV();
void saveCSV();

// dsa.c (Algorithms)
void searchRecords(char *query);
void binarySearchByDate(char *targetDate);
void quickSort(WeatherRecord arr[], int low, int high, int key, bool ascending);
void showTop5(bool hottest);
void showStormImpact();
void predictWeather();
void initGraphFromData(); // NEW: Generates graph from loaded CSV data
void updateRecord(int id, float newTemp, int newHumidity);
void extractUniqueCities(char cities[][CITY_LEN], int *cityCount);
void showTrendJSON(char *city, char *date);
void generateDashboardJSON();
void showTop5PerCity(bool hottest);
void showTop5city_records(bool hottest, char *city);
void showStormImpactForCity(char *city);
void showStormImpactForCityJSON(char *epicenterCity);

// main.c (UI Helpers)
void clearScreen();
void pause();
void showDashboard();
void runFrontend();

#endif