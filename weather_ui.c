#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX 200

/* ---------------- DATA STRUCTURE ---------------- */
typedef struct {
    int id;
    char city[20];
    float temp;
    int humidity;
    char date[12];
} WeatherRecord;

WeatherRecord records[MAX];
int count = 0;

/* ---------------- UI HANDLES ---------------- */
HWND hOutput, hCityInput;

/* ---------------- LOAD CSV ---------------- */
void loadCSV() {
    FILE *fp = fopen("weather.csv", "r");
    if (!fp) return;

    char header[200];
    fgets(header, sizeof(header), fp); // skip header

    while (fscanf(fp, "%d,%[^,],%f,%d,%s",
                  &records[count].id,
                  records[count].city,
                  &records[count].temp,
                  &records[count].humidity,
                  records[count].date) != EOF) {
        count++;
    }
    fclose(fp);
}

/* ---------------- DISPLAY ALL ---------------- */
void displayAll() {
    char buffer[4000] = "City   Temp   Humidity   Date\r\n";
    strcat(buffer, "--------------------------------\r\n");

    for (int i = 0; i < count; i++) {
        char line[200];
        sprintf(line, "%-8s %.1fC   %d%%        %s\r\n",
                records[i].city,
                records[i].temp,
                records[i].humidity,
                records[i].date);
        strcat(buffer, line);
    }
    SetWindowText(hOutput, buffer);
}

/* ---------------- SORT BY TEMPERATURE ---------------- */
void sortByTemperature() {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (records[j].temp > records[j + 1].temp) {
                WeatherRecord t = records[j];
                records[j] = records[j + 1];
                records[j + 1] = t;
            }
        }
    }
}

/* ---------------- TOP 5 HOTTEST ---------------- */
void top5Hottest() {
    sortByTemperature();
    char buffer[1500] = "Top 5 Hottest Days\r\n------------------\r\n";

    for (int i = count - 1; i >= count - 5 && i >= 0; i--) {
        char line[200];
        sprintf(line, "%s  %.1fC  %s\r\n",
                records[i].city,
                records[i].temp,
                records[i].date);
        strcat(buffer, line);
    }
    SetWindowText(hOutput, buffer);
}

/* ---------------- TOP 5 COLDEST ---------------- */
void top5Coldest() {
    sortByTemperature();
    char buffer[1500] = "Top 5 Coldest Days\r\n------------------\r\n";

    for (int i = 0; i < 5 && i < count; i++) {
        char line[200];
        sprintf(line, "%s  %.1fC  %s\r\n",
                records[i].city,
                records[i].temp,
                records[i].date);
        strcat(buffer, line);
    }
    SetWindowText(hOutput, buffer);
}

/* ---------------- SEARCH BY CITY ---------------- */
void searchCity() {
    char city[20];
    GetWindowText(hCityInput, city, 20);

    char buffer[1500] = "Search Result\r\n-------------\r\n";
    int found = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(records[i].city, city) == 0) {
            char line[200];
            sprintf(line, "%s  %.1fC  %d%%  %s\r\n",
                    records[i].city,
                    records[i].temp,
                    records[i].humidity,
                    records[i].date);
            strcat(buffer, line);
            found = 1;
        }
    }

    if (!found)
        strcat(buffer, "No records found.\r\n");

    SetWindowText(hOutput, buffer);
}

/* ---------------- WINDOW PROCEDURE ---------------- */
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_CREATE:
        loadCSV();

        CreateWindow("STATIC", "City:",
            WS_VISIBLE | WS_CHILD,
            20, 20, 40, 20,
            hwnd, NULL, NULL, NULL);

        hCityInput = CreateWindow("EDIT", "",
            WS_VISIBLE | WS_CHILD | WS_BORDER,
            70, 18, 120, 22,
            hwnd, NULL, NULL, NULL);

        CreateWindow("BUTTON", "Search City",
            WS_VISIBLE | WS_CHILD,
            210, 16, 100, 26,
            hwnd, (HMENU)1, NULL, NULL);

        CreateWindow("BUTTON", "View All",
            WS_VISIBLE | WS_CHILD,
            20, 60, 100, 30,
            hwnd, (HMENU)2, NULL, NULL);

        CreateWindow("BUTTON", "Sort by Temp",
            WS_VISIBLE | WS_CHILD,
            140, 60, 120, 30,
            hwnd, (HMENU)3, NULL, NULL);

        CreateWindow("BUTTON", "Top 5 Hottest",
            WS_VISIBLE | WS_CHILD,
            280, 60, 130, 30,
            hwnd, (HMENU)4, NULL, NULL);

        CreateWindow("BUTTON", "Top 5 Coldest",
            WS_VISIBLE | WS_CHILD,
            280, 100, 130, 30,
            hwnd, (HMENU)5, NULL, NULL);

        hOutput = CreateWindow("EDIT", "",
            WS_VISIBLE | WS_CHILD | WS_BORDER |
            ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL,
            20, 140, 390, 260,
            hwnd, NULL, NULL, NULL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1: searchCity(); break;
        case 2: displayAll(); break;
        case 3: sortByTemperature(); displayAll(); break;
        case 4: top5Hottest(); break;
        case 5: top5Coldest(); break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

/* ---------------- MAIN ---------------- */
int WINAPI WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInst,
    LPSTR lpCmdLine,
    int nCmdShow
) 
 {

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "WeatherApp";

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "WeatherApp",
        "Weather Analysis System (Pure C WinAPI)",
        WS_OVERLAPPEDWINDOW,
        200, 100, 460, 460,
        NULL, NULL, hInst, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}











