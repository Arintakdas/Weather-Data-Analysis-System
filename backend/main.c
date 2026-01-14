#include "weather.h"
#include <stdio.h>

WeatherRecord db[MAX_RECORDS];
int count = 0;

const char *CITY_NODES[MAX_CITIES] = {
    "Mumbai",
    "Delhi",
    "Bangalore",
    "Hyderabad",
    "Ahmedabad",
    "Chennai",
    "Kolkata",
    "Surat",
    "Pune",
    "Jaipur",
    "Lucknow",
    "Kanpur",
    "Nagpur",
    "Indore",
    "Thane",
    "Bhopal",
    "Visakhapatnam",
    "Pimpri-Chinchwad",
    "Patna",
    "Vadodara",
    "Ghaziabad",
    "Ludhiana",
    "Agra",
    "Nashik",
    "Faridabad",
    "Meerut",
    "Rajkot",
    "Kalyan-Dombivli",
    "Vasai-Virar",
    "Varanasi",
    "Srinagar",
    "Aurangabad",
    "Dhanbad",
    "Amritsar",
    "Navi Mumbai",
    "Allahabad",
    "Ranchi",
    "Howrah",
    "Coimbatore",
    "Jabalpur",
    "Gwalior",
    "Vijayawada",
    "Jodhpur",
    "Madurai",
    "Raipur",
    "Kota",
    "Chandigarh",
    "Guwahati",
    "Solapur",
    "Hubli-Dharwad",
    "Tiruchirappalli",
    "Bareilly",
    "Mysore",
    "Tiruppur",
    "Gurgaon",
    "Aligarh",
    "Jalandhar",
    "Bhubaneswar",
    "Salem",
    "Mira-Bhayandar",
    "Warangal",
    "Thiruvananthapuram",
    "Guntur",
    "Bhiwandi",
    "Saharanpur",
    "Gorakhpur",
    "Bikaner",
    "Amravati",
    "Noida",
    "Jamshedpur",
    "Bhilai",
    "Cuttack",
    "Firozabad",
    "Kochi",
    "Nellore",
    "Bhavnagar",
    "Dehradun",
    "Durgapur",
    "Asansol",
    "Rourkela",
    "Nanded",
    "Kolhapur",
    "Ajmer",
    "Akola",
    "Gulbarga",
    "Jamnagar",
    "Ujjain",
    "Loni",
    "Siliguri",
    "Jhansi",
    "Ulhasnagar",
    "Jammu",
    "Sangli-Miraj",
    "Mangalore",
    "Erode",
    "Belgaum",
    "Ambattur",
    "Tirunelveli",
    "Malegaon",
    "Gaya",
    "Jalgaon",
    "Udaipur",
    "Maheshtala",
    "Davanagere",
    "Kozhikode",
    "Kurnool",
    "Rajpur Sonarpur",
    "Rajahmundry",
    "Bokaro",
    "South Dumdum",
    "Bellary",
    "Patiala",
    "Gopalpur",
    "Agartala",
    "Bhagalpur",
    "Muzaffarnagar",
    "Bhatpara",
    "Panihati",
    "Latur",
    "Dhule",
    "Tirupati",
    "Rohtak",
    "Korba",
    "Bhilwara",
    "Berhampur",
    "Muzaffarpur",
    "Ahmednagar",
    "Mathura",
    "Kollam",
    "Avadi",
    "Kadapa",
    "Kamarhati",
    "Sambalpur",
    "Bilaspur",
    "Shahjahanpur",
    "Satara",
    "Bijapur",
    "Rampur",
    "Shivamogga",
    "Chandrapur",
    "Junagadh",
    "Thrissur",
    "Alwar",
    "Bardhaman",
    "Kulti",
    "Kakinada",
    "Nizamabad",
    "Parbhani",
    "Tumkur",
    "Khammam",
    "Ozhukarai",
    "Bihar Sharif",
    "Panipat",
    "Darbhanga",
    "Bally",
    "Aizawl",
    "Dewas",
    "Ichalkaranji",
    "Karnal",
    "Bathinda",
    "Jalna",
    "Eluru",
    "Kirari Suleman Nagar",
    "Barasat",
    "Purnia",
    "Satna",
    "Mau",
    "Sonipat",
    "Farrukhabad",
    "Sagar",
    "Rourkela",
    "Durg",
    "Imphal",
    "Ratlam",
    "Hapur",
    "Arrah",
    "Karimnagar",
    "Anantapur",
    "Etawah",
    "Ambernath",
    "North Dumdum",
    "Bharatpur",
    "Begusarai",
    "New Delhi",
    "Gandhidham",
    "Baranagar",
    "Tiruvottiyur",
    "Puducherry",
    "Sikar",
    "Thoothukudi",
    "Rewa",
    "Mirzapur",
    "Raichur",
    "Pali",
    "Ramagundam",
    "Haridwar",
    "Vijayanagaram",
    "Katihar",
    "Naihati",
    "Sambhal",
    "Nadiad",
    "Yamunanagar",
    "English Bazar",
    "Unnao",
    "Khandwa",
    "Morbi",
    "Bhusawal",
    "Orai",
    "Bahraich",
    "Vellore",
    "Mahesana",
    "Sambalpur",
    "Raiganj",
    "Sirsa",
    "Danapur",
    "Serampore",
    "Sultan Pur Majra",
    "Guna",
    "Jaunpur",
    "Panvel",
    "Shivpuri",
    "Surendranagar Dudhrej",
    "Unnao",
    "Hugli-Chinsurah",
    "Alappuzha",
    "Kottayam",

};

int adjMatrix[MAX_CITIES][MAX_CITIES] = {0};

void initializeAdjacencyMatrix() {
  adjMatrix[0][8] = 1;
  adjMatrix[0][23] = 1;
adjMatrix[0][14] = 1; 

  adjMatrix[1][54] = 1;
  adjMatrix[1][68] = 1;
  adjMatrix[1][20] = 1;
  adjMatrix[1][24] = 1;

  adjMatrix[2][52] = 1;
}

int main(int argc, char *argv[]) {
  initializeAdjacencyMatrix();

  if (argc < 2) {
    runFrontend();
    return 0;
  }

  char *cmd = argv[1];

  if (strcmp(cmd, "dashboard") == 0) {
    loadCSV();
    showDashboard();
  } else if (strcmp(cmd, "dashboard_json") == 0) {
    loadCSV();
    printDashboardJSON();
  } else if (strcmp(cmd, "load") == 0) {
    loadCSV();
  } else if (strcmp(cmd, "save") == 0) {
    loadCSV();
    saveCSV();
  } else if (strcmp(cmd, "search_city") == 0) {
    loadCSV();
    if (argc >= 3) {
      searchRecords(argv[2]);
    } else {
      printf("Usage: search_city <city_name>\n");
    }
  } else if (strcmp(cmd, "search_date") == 0) {
    loadCSV();
    if (argc >= 3) {
      binarySearchByDate(argv[2]);
    } else {
      printf("Usage: search_date <YYYY-MM-DD>\n");
    }
  } else if (strcmp(cmd, "top5") == 0) {
    loadCSV();
    if (argc >= 3) {
      bool hottest = (strcmp(argv[2], "hottest") == 0);
      showTop5(hottest);
    } else {
      showTop5(true);
    }
  } else if (strcmp(cmd, "top5percity") == 0) {
    loadCSV();
    if (argc >= 3) {
      bool hottest = (strcmp(argv[2], "hottest") == 0);
      showTop5PerCity(hottest);
    } else {
      showTop5PerCity(true);
    }
  } else if (strcmp(cmd, "storm") == 0) {
    if (argc >= 3) {
      int idx = atoi(argv[2]);
      printf("%d", idx);
      getchar();
      showStormImpactAt(idx);
    } else {
      printf("Usage: storm <city_index>\n");
    }
  } else {
    printf("Unknown command: %s\n", cmd);
    printf("Available commands:\n");
    printf("  dashboard\n");
    printf("  load\n");
    printf("  save\n");
    printf("  search_city <name>\n");
    printf("  search_date <YYYY-MM-DD>\n");
    printf("  top5 <hottest|coldest>\n");
    printf("  top5percity <hottest|coldest>\n");
    printf("  storm <index>\n");
  }

  return 0;
}
