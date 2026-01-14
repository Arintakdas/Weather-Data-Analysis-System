# Weather Data Analysis System

A comprehensive weather analytics application with C backend and Node.js web frontend.

## Quick Start

### 1. Compile the Backend

Open a terminal in the `backend` directory:

```bash
cd backend
gcc -o weather.exe main.c dsa.c -std=c11
```

Or simply double-click `compile.bat` (Windows).

### 2. Start the Frontend Server

Open a terminal in the `frontend_web` directory:

```bash
cd frontend_web
node server.js
```

Or simply double-click `start.bat` (Windows).

### 3. Open Your Browser

Navigate to: **http://localhost:3000**

## Features

- **Dashboard**: View summary statistics of weather data
- **Search by City**: Find weather records for specific cities
- **Search by Date**: Find weather records for specific dates
- **Top 5 Hottest/Coldest**: See the extreme temperature readings overall
- **Top 5 Per City**: See the top 5 hottest or coldest days for each individual city
- **Storm Warning System**: Predict storm propagation using graph algorithms
- **Save Data**: Export current database to CSV

## Architecture

- **Backend**: C (using data structures like arrays, QuickSort, adjacency matrix)
- **Frontend**: Node.js with Express serving static HTML/CSS/JavaScript
- **Communication**: REST API calling C executable with command-line arguments

## Requirements

- **GCC** or MinGW (for compiling C code)
- **Node.js** (v12 or higher)
- **npm** (comes with Node.js)

## Data File

Place your weather data CSV file as `weather_data.csv` in the `backend` directory.

Expected CSV format:
- Column 2: Date
- Column 3: City Name
- Column 4: Humidity
- Column 20: Temperature (at noon)

## License

Educational project for Data Structures and Algorithms demonstration.
