#include "weather.h"
#include <stdlib.h>

void trimNewline(char *s) { s[strcspn(s, "\r\n")] = '\0'; }

void saveCSV() {
  FILE *fp = fopen("weather_data_saved.csv", "w");
  if (fp == NULL) {
    printf(COLOR_RED "[ERROR] Could not open file for writing.\n" COLOR_RESET);
    return;
  }

  fprintf(fp, "RecordID,City,Temperature,Humidity,Date\n");
  for (int i = 0; i < count; i++) {
    fprintf(fp, "%d,%s,%.2f,%d,%s\n", db[i].id, db[i].city, db[i].temperature,
            db[i].humidity, db[i].date);
  }

  fclose(fp);
  printf(COLOR_GREEN "[SUCCESS] Saved %d records to disk.\n" COLOR_RESET,
         count);
}

void loadCSV() {
  FILE *fp = fopen("india_216_cities_hourlytemps_dailyrows_1year.csv", "r");

  if (!fp) {
    fp = fopen("weather_data_saved.csv", "r");
  }

  if (!fp) {
    printf(COLOR_YELLOW "[INFO] No database file found. Starting with 0 "
                        "records.\n" COLOR_RESET);
    return;
  }

  char line[4096];
  int row = 0;
  count = 0;

  printf("[LOADING] Parsing dataset... please wait.\n");

  fgets(line, sizeof(line), fp);

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

      if (col == 2)
        date_str = token;
      if (col == 3)
        city_str = token;
      if (col == 4)
        hum_str = token;
      if (col == 20)
        temp_str = token;

      token = strtok(NULL, ",");
      col++;
    }

    if (city_str && date_str && temp_str) {
      WeatherRecord r;
      r.id = count + 1;

      trimNewline(city_str);
      if (city_str[0] == '"')
        city_str++;
      char *q = strrchr(city_str, '"');
      if (q)
        *q = 0;
      strncpy(r.city, city_str, CITY_LEN);

      trimNewline(date_str);
      if (date_str[0] == '"')
        date_str++;
      q = strrchr(date_str, '"');
      if (q)
        *q = 0;
      strncpy(r.date, date_str, DATE_LEN);

      r.temperature = atof(temp_str);
      r.humidity = (hum_str) ? atoi(hum_str) : 0;

      db[count++] = r;
    }
  }

  fclose(fp);
  printf(COLOR_GREEN "[SUCCESS] Loaded %d records into memory.\n" COLOR_RESET,
         count);
}