# Weather-Data-Analysis-System
Weather Data Analysis System using C programming
1. Problem Statement
Create a Weather Data Analysis System using array-based sorting and searching
algorithms.
2. Detailed Specifications
 Store weather dataset for multiple cities over time.
 # Implement:
o Search by date/city

o Sort by temperature/humidity

o Find top 5 hottest/coldest days

o Graph generation (optional)

      Must be menu-driven.

3. Dataset Structure

Table: WeatherRecords

RecordID (INT)

City (VARCHAR)


---

## Pictorial Representation

### 1. System Flow Diagram

The following flow diagram represents the overall working of the Weather Data Analysis System:

    +--------------------+
    | Start Program |
    +--------------------+
    |
    v
    +--------------------+
    | Display Main Menu |
    +--------------------+
    |
    v
    +----------------------------+
    | User Selects an Option |
    +----------------------------+
    |
    v
    +----------------------------------------------+
    | Add / Display / Search / Sort / Analyze Data |
    +----------------------------------------------+
    |
    v
    +--------------------+
    | Exit Program |
    +--------------------+


---

### 2. Data Flow Representation

This diagram shows how weather data flows through the system:

    User Input
    |
    v
    +------------------+
    | Weather Record |
    | (Structure) |
    +------------------+
    |
    v
    +------------------+
    | Array Storage |
    +------------------+
    |
    v
    +-----------------------------+
    | Search / Sort / Analysis |
    +-----------------------------+
    |
    v
    Processed Output (Console)



---

### 3. Menu-Driven Program Structure

    +-----------------------------+
    | MAIN MENU |
    +-----------------------------+
    | 1. Add Weather Record |
    | 2. Display Records | 
    | 3. Search by City |  
    | 4. Search by Date |  
    | 5. Sort by Temperature |  
    | 6. Sort by Humidity |   
    | 7. Top 5 Hottest Days |    
    | 8. Top 5 Coldest Days |    
    | 9. Exit |
    +-----------------------------+



---

### 4. Sample Temperature Analysis (Text-Based Graph)

   Example of temperature comparison using a text-based representation:
    
    Date Temperature (°C)
    01-04-2024 ||||||||||||| 32
    02-04-2024 ||||||||||||||| 34
    03-04-2024 ||||||||||| 29   
    04-04-2024 ||||||||||||||||| 36
    05-04-2024 |||||||||| 27

(Note: Graphical output is optional and represented using text.)

---

### 5. Structure Representation in C

struct WeatherRecord {

int recordID;

char city[50];

float temperature;

int humidity;

char date[15];

};

---
