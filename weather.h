#ifndef WEATHER_H
#define WEATHER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define MAX_RECORDS 200
#define CITY_LEN 50
#define DATE_LEN 15
#define FILE_NAME "weather_data.csv"
#define MAX_CITIES 25


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


extern WeatherRecord db[MAX_RECORDS];
extern int count;
extern const char* CITY_NODES[MAX_CITIES];
extern int adjMatrix[MAX_CITIES][MAX_CITIES];

void loadCSV();
void saveCSV();
void clearScreen();
void pause();
void showDashboard();
void predictWeather();

void searchRecords(char *query);
void quickSort(WeatherRecord arr[], int low, int high, int key, bool ascending);
void showTop5(bool hottest);
void showStormImpact(); 
#endif
