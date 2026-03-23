#include "weather.h"
#include <stdlib.h>

void trimNewline(char *s) { s[strcspn(s, "\r\n")] = '\0'; }

void saveCSV() {
  FILE *fp = fopen("weather_data_saved.csv", "w");
  if (fp == NULL) {
    printf(COLOR_RED "[ERROR] Could not open file for writing.\n" COLOR_RESET);
    return;
  }

  fprintf(fp, "RecordID,CityID,Date,City,Humidity,WindSpeed_kmh,Rainfall_mm,AQI,Temp00,Temp01,Temp02,Temp03,Temp04,Temp05,Temp06,Temp07,Temp08,Temp09,Temp10,Temp11,Temp12,Temp13,Temp14,Temp15,Temp16,Temp17,Temp18,Temp19,Temp20,Temp21,Temp22,Temp23\n");
  for (int i = 0; i < count; i++) {
    fprintf(fp, "%d,1,%s,%s,%d,0.0,0.0,50", db[i].id, db[i].date, db[i].city, db[i].humidity);
    for(int h=0; h<24; h++) {
        fprintf(fp, ",%.2f", db[i].hourly_temps[h]);
    }
    fprintf(fp, "\n");
  }

  fclose(fp);
  printf(COLOR_GREEN "[SUCCESS] Saved %d records to disk.\n" COLOR_RESET,
         count);
}

void loadCSV() {
    FILE *fp = fopen("weather_data_saved.csv", "r");

    if (!fp) {
        if (!is_cli_mode) {
            printf(COLOR_YELLOW
                   "[INFO] No database file found. Starting with 0 records.\n"
                   COLOR_RESET);
        }
        return;
    }

    char line[4096];
    count = 0;

    if (!is_cli_mode) {
        printf("[LOADING] Parsing dataset... please wait.\n");
    }

    // Skip header
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {

        if (count >= MAX_RECORDS)
            break;

        char *token;
        int col = 0;

        char *date_str = NULL;
        char *city_str = NULL;
        char *hum_str  = NULL;
        char *hourly_strs[24] = {NULL};

        token = strtok(line, ",");

        while (token) {
            if (col == 2) date_str = token;
            else if (col == 3) city_str = token;
            else if (col == 4) hum_str = token;
            else if (col >= 8 && col <= 31) {
                hourly_strs[col - 8] = token;
            }
            token = strtok(NULL, ",");
            col++;
        }

        if (!city_str || !date_str)
            continue;

        WeatherRecord r;
        r.id = count + 1;
        for(int h=0; h<24; h++) r.hourly_temps[h] = 0.0f;

        trimNewline(city_str);
        strncpy(r.city, city_str, CITY_LEN);

        trimNewline(date_str);
        strncpy(r.date, date_str, DATE_LEN);

        r.humidity = hum_str ? atoi(hum_str) : 0;
        
        float sum = 0;
        int tCount = 0;
        for (int h = 0; h < 24; h++) {
            if (hourly_strs[h]) {
                r.hourly_temps[h] = atof(hourly_strs[h]);
                sum += r.hourly_temps[h];
                tCount++;
            }
        }
        r.temperature = (tCount > 0) ? (sum / tCount) : 0.0f;

        db[count++] = r;
    }

    fclose(fp);

    if (!is_cli_mode) {
        printf(COLOR_GREEN "[SUCCESS] Loaded %d records into memory.\n" COLOR_RESET,
               count);
    }
}
