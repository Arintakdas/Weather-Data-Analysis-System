# Frontend Workflow & Architecture Documentation

This document outlines the workflow, files, and core functions powering the web-based graphical interface for the Weather Analytics System.

## 1. System Overview
The frontend is split into two layers:
1. **The Node.js API Web Server** (`server.js`)
2. **The Client-Side UI Environment** (`index.html`, `main.js`, `styles.css`)

---

## 2. The Node.js API Server (`server.js`)
Since the browser cannot natively run C applications or parse local CSV files, a lightweight Express server is used as a secure bridge.

### Core Logic
*   **`runExe(args, res)`**: The vital helper function. It uses Node's `child_process.execFile` to spawn a temporary hidden instance of `backend/weather.exe`, passes it CLI arguments, waits until the C program finishes running, captures the text output (`stdout`), and packages it into a JSON response to send to the browser.
*   **API Endpoints**: 
    *   `/api/dashboard_json` (Global KPI stats)
    *   `/api/trend?city=X&date=Y` (24-Hour chart data)
    *   `/api/top5?type=X&city=Y` (Top 5 queries via table parsing)
    *   `/api/storm_json?idx=X` (Graph adjacencies)

**Workflow**: Browser clicks button -> fetch(`/api/...`) -> Express routes it -> spawns `weather.exe` -> backend prints data -> Express captures it -> Express sends JSON back to Browser.

---

## 3. Client-Side Interface (`index.html` & `main.js`)
The user interface is built as a Single Page Application (SPA). It never refreshes the page; instead, it uses Javascript to hide/show virtual "tabs" and dynamically fetch data.

### Presentation & Structure (`index.html` & `styles.css`)
*   **Glassmorphic UI**: Uses custom CSS variables, gradients, and backdrop-filters to create a modern MSN-style weather dashboard.
*   **Tab Views**: Includes `#view-overview` (widgets/graphs), `#view-data` (table explorer), `#view-radar` (storm impact visualization), and `#view-system` (save/load IO controls).

### Client State & Logic (`main.js`)
*   **`api(path, gotoTab)`**: A master abstraction wrapper around the native browser `fetch()`. It:
    1. Shows a "Loading..." indicator.
    2. Calls the Node.js server asynchronously.
    3. Triggers the text-to-table parser if traditional column data is returned.
    4. Auto-switches the UI tab smoothly.
*   **`parseTable(text)`**: A dynamic RegEx parsing engine. Since many older C backend functions return pipe-separated text tables (e.g. `City | Temp | Date`), this function reads that raw block, strips out the Terminal color ANSI codes (`\u001b`), dynamically detects column counts, and rebuilds proper HTML `<tr>` and `<td>` elements in the Database Explorer interface.
*   **`fetchDashboard()` & `renderDashboard(d)`**: Automatically fires when the app loads. Queries `/api/dashboard_json`, extracts average/extremes from the C backend output, and paints them into the big Hero KPI cards.
*   **`plotTrend(city, date)`**: Queries `/api/trend`, parses the 24-hour temperature float array from the C backend, and utilizes the external **Chart.js** library to smoothly render an interactive visual trendline. It ALSO patches the Hero card's Average Temp with the newly fetched data.
*   **`drawRadar(data)`**: Feeds JSON neighbor adjacency data into the external **Vis.js Network** library to visualize connected, at-risk cities dynamically in an interactive node/spring diagram.

### Interactions & Events
Standard DOM Event Listeners link every UI button to an API call. For example:
```javascript
document.getElementById('btnTopHotCard').onclick = () => {
  const city = document.getElementById('top5CityInput').value.trim();
  const url = '/api/top5?type=hottest' + (city ? '&city=' + encodeURIComponent(city) : '');
  api(url, 'data');
};
```