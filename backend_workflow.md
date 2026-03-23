# Backend Workflow & Function Documentation

This document provides a comprehensive overview of the C Backend process, detailing the core functions and how data flows through the application.

## 1. System Overview & Entry Point (`main.c`)
The backend acts as the core data engine. It operates in two modes:
*   **Interactive Menu Mode:** Launched when the `.exe` is run without arguments. Provides a terminal UI.
*   **CLI / API Mode:** Launched by the Node.js frontend using arguments (e.g., `weather.exe dashboard_json`). It silently executes a single command and returns parsable output (usually JSON or Pipe-separated text).

### Core Functions in `main.c`
*   **`main(int argc, char *argv[])`**: The entry point. It first calls `loadCSV()` and `initGraphFromData()`, and then either parses CLI arguments to trigger specific outputs (CLI Mode) or calls `runFrontend()` (Interactive Mode).
*   **`runFrontend()`**: Renders the terminal-based interactive menu system and handles user input.
*   **`showDashboard()`**: Prints a quick terminal overview of the latest 20 records.
*   **`clearScreen()` & `pause()`**: Terminal UI helpers for better readability.

---

## 2. Data Persistence (`file_io.c`)
Data is stored permanently in `weather_data.csv`. The system loads this into a massive static/memory array `extern WeatherRecord db[MAX_RECORDS]` on startup.

### Core Functions in `file_io.c`
*   **`loadCSV()`**: Opens `weather_data.csv`, parses each line into the global `db` array, and keeps track of the total `count`. Also extracts the 24 hourly temperatures for each day.
*   **`saveCSV()`**: Writes the entire current memory state (`db` array) back to the CSV file, overwriting the old file.

---

## 3. Data Structures & Algorithms (`dsa.c`)
This file is the brain of the backend, containing all complex data manipulation, sorting, searching, and graph mapping logic.

### Searching & Filtering
*   **`searchRecords(char *query)`**: Performs a linear search across the database for a matching `city` (case-insensitive) or `date`. Outputs results in a table.
*   **`binarySearchByDate(char *targetDate)`**: Searches for all records matching a specific date (auto-converts YYYY-MM-DD to DD-MM-YYYY) and prints the 24-hour temperature breakdown.

### Sorting & Analytics
*   **`quickSort(WeatherRecord arr[], int low, int high, int key, bool ascending)`**: Custom QuickSort implementation. Sorts records by Temperature or Date cleanly.
*   **`showTop5(bool hottest)`**: Sorts the *entire* global dataset and prints the absolute top 5 hottest or coldest days across all cities.
*   **`showTop5PerCity(bool hottest)`**: Finds the peak highest/lowest temperature recorded for *each unique city*, sorts those peaks, and outputs the overall Top 5 hottest/coldest cities globally.
*   **`showTop5city_records(bool hottest, char *city)`**: Extracts all records for a *specific user-provided city* into a temporary heap array, sorts them, and outputs the Top 5 days purely for that city.

### Graph & Network Simulation
*   **`initGraphFromData()`**: Iterates through the database to find all unique cities, mapping them to `CITY_NODES`. It then randomly generates an Adjacency Matrix (`adjMatrix`) simulating geographic connections between them.
*   **`showStormImpactForCityJSON(char *epicenterCity)`**: Given a target city, checks the `adjMatrix` for all directly connected neighboring cities and outputs a JSON response listing the cities at risk from a spreading storm.

### JSON APIs (Frontend Interfacing)
*   **`generateDashboardJSON()`**: Scans all records to calculate averages and extremes, returning a clean JSON object used to populate the frontend's top KPI cards.
*   **`showTrendJSON(char *city, char *date)`**: Fetches a single day's 24-hour hourly temperatures for a specific city and formats the output into a JSON object `data.hourly` for Chart.js rendering on the frontend.

### AI / Prediction (Prototype)
*   **`predictWeather()`**: A basic Linear Regression prototype that calculates the average rate of absolute temperature change over the most recent 100 records to predict tomorrow's baseline temperature.

---

## 4. End-to-End Execution Workflow
1. **Boot**: `node server.js` receives a request on `/api/X`.
2. **Execute**: It spawns `weather.exe [arguments]`.
3. **Initialize Memory**: `main()` runs `loadCSV()` (populates `db[79000]`) and `initGraphFromData()` (populates `CITY_NODES[300]`).
4. **Process Command**: `main()` routes the argument to the right function in `dsa.c`.
5. **Output**: The C program `printf()`s JSON or formatted strings to standard output.
6. **Terminate**: The C process immediately exits (`return 0`).
7. **Frontend Yield**: The Node.js server captures the standard output, bundles it, and sends it back to the browser.
