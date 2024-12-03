#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <cctype>
#include <set>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Flight {  // struct to hold flight data
    string carrier;
    string airport_name;
    int arr_delay;
};

string trim(const string& str) {  // function to trim whitespace
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

vector<string> parseCSVLine(const string& line, char delimiter) {  // function to parse CSV line
    vector<string> tokens;
    string token;
    bool inside_quotes = false;

    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];

        if (c == '"') {
            if (inside_quotes && i + 1 < line.length() && line[i + 1] == '"') {
                token += '"';  // escaped quote inside quoted field
                ++i;
            } else {
                inside_quotes = !inside_quotes;  // toggle quotes flag
            }
        } else if (c == delimiter && !inside_quotes) {
            tokens.push_back(trim(token));  // delimiter outside quotes
            token.clear();
        } else {
            token += c;
        }
    }
    tokens.push_back(trim(token));
    return tokens;
}

vector<Flight> readFlightData(const string& filename) {  // function to read flight data
    vector<Flight> flights;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << endl;
        return flights;
    }

    string line;
    if (!getline(file, line)) {  // read header line
        cerr << "Failed to read header line from the file." << endl;
        return flights;
    }

    char comma = ',';  // detect comma
    if (line.find('\t') != string::npos) {
        comma = '\t';
    }

    vector<string> headers = parseCSVLine(line, comma);

    cout << "Headers:" << endl;
    for (size_t i = 0; i < headers.size(); ++i) {  // output headers for debugging
        cout << i << ": '" << headers[i] << "'" << endl;
    }

    int carrier_idx = -1, airport_name_idx = -1, arr_delay_idx = -1;
    for (size_t i = 0; i < headers.size(); ++i) {  // find relevant column indices
        string header_lower = headers[i];
        transform(header_lower.begin(), header_lower.end(), header_lower.begin(), ::tolower);
        header_lower = trim(header_lower);
        if (header_lower == "carrier") carrier_idx = i;
        else if (header_lower == "airport_name") airport_name_idx = i;
        else if (header_lower == "arr_delay") arr_delay_idx = i;
    }

    cout << "carrier_idx: " << carrier_idx << ", airport_name_idx: " << airport_name_idx << ", arr_delay_idx: " << arr_delay_idx << endl;

    if (carrier_idx == -1 || airport_name_idx == -1 || arr_delay_idx == -1) {  // check for missing columns
        cerr << "Required columns not found in the CSV file." << endl;
        return flights;
    }

    while (getline(file, line)) {  // read flight data
        if (line.empty()) continue;

        vector<string> tokens = parseCSVLine(line, comma);

        if (tokens.size() <= max({carrier_idx, airport_name_idx, arr_delay_idx})) {  // skip invalid lines
            continue;
        }

        Flight flight;
        flight.carrier = tokens[carrier_idx];
        flight.airport_name = tokens[airport_name_idx];

        try {
            flight.arr_delay = static_cast<int>(stod(tokens[arr_delay_idx]));  // parse arr_delay
        } catch (const invalid_argument& e) {
            continue;
        }
        flights.push_back(flight);
    }

    file.close();
    return flights;
}

void quickSort(vector<Flight>& flights, int low, int high) {
    if (low < high) {
        // random pivot selection
        int pivotIndex = low + rand() % (high - low + 1);
        swap(flights[pivotIndex], flights[high]); // move pivot to end

        int pivot = flights[high].arr_delay;
        int i = low - 1;
        for (int j = low; j < high; ++j) {
            if (flights[j].arr_delay <= pivot) {
                ++i;
                swap(flights[i], flights[j]);
            }
        }
        swap(flights[i + 1], flights[high]);
        int pi = i + 1;

        quickSort(flights, low, pi - 1);
        quickSort(flights, pi + 1, high);
    }
}


// Placeholder for merge sort implementation
void mergeSort(vector<Flight>& flights, int left, int right) {
    // FIXME: implement merge sort here
    // function should sort the flights vector between indices left and right
    // based on arr_delay using the merge sort algorithm. make sure to check out comments in int main as well
}

vector<Flight> getBestCase(const vector<Flight>& flights) {  // best case (sorted)
    vector<Flight> sortedFlights = flights;
    sort(sortedFlights.begin(), sortedFlights.end(), [](const Flight& a, const Flight& b) {
        return a.arr_delay < b.arr_delay;
    });
    return sortedFlights;
}

vector<Flight> getWorstCase(const vector<Flight>& flights) {  // worst case (reverse sorted)
    vector<Flight> sortedFlights = flights;
    sort(sortedFlights.begin(), sortedFlights.end(), [](const Flight& a, const Flight& b) {
        return a.arr_delay > b.arr_delay;
    });
    return sortedFlights;
}

vector<Flight> getAverageCase(const vector<Flight>& flights) {  // average case (shuffled)
    vector<Flight> shuffledFlights = flights;
    random_shuffle(shuffledFlights.begin(), shuffledFlights.end());
    return shuffledFlights;
}

int main() {
    string filename = "C:/Users/peyto/Airline_Delay_Cause.csv";
    vector<Flight> flights = readFlightData(filename);

    if (flights.empty()) {  // check if data is empty
        cerr << "No data to sort." << endl;
        return 1;
    }

    set<string> validCarriers;
    for (const auto& flight : flights) {  // collect valid airline names
        validCarriers.insert(flight.carrier);
    }

    int sortingMethod = 0;
    cout << "Select the sorting method to test:\n";
    cout << "1. Quick Sort\n";
    cout << "2. Merge Sort\n";
    cout << "Enter your choice (1 or 2): ";
    cin >> sortingMethod;

    while (sortingMethod != 1 && sortingMethod != 2) {  // validate sorting method
        cout << "Invalid choice. Please enter 1 or 2: ";
        cin >> sortingMethod;
    }

    int filterChoice = 0;
    cout << "Do you want to sort delays specific to:\n";
    cout << "1. An airline carrier\n";
    cout << "2. An airport (city name)\n";
    cout << "Enter your choice (1 or 2): ";
    cin >> filterChoice;

    while (filterChoice != 1 && filterChoice != 2) {  // validate filter choice
        cout << "Invalid choice. Please enter 1 or 2: ";
        cin >> filterChoice;
    }

    vector<Flight> selectedFlights;

    if (filterChoice == 1) {  // filter by airline
        string airlineName;
        cout << "Enter the airline carrier code (e.g., AA, DL, UA): ";
        cin >> airlineName;

        while (validCarriers.find(airlineName) == validCarriers.end()) {  // validate airline code
            cout << "Invalid airline carrier code. Please enter a valid airline code: ";
            cin >> airlineName;
        }

        for (const auto& flight : flights) {  // filter flights by carrier
            if (flight.carrier == airlineName) {
                selectedFlights.push_back(flight);
            }
        }

        if (selectedFlights.empty()) {
            cout << "No flights found for the airline: " << airlineName << endl;
            return 1;
        }
    } else if (filterChoice == 2) {  // filter by airport
        string airportName;
        cout << "Enter the airport city name (e.g., Chicago, Birmingham): ";
        cin.ignore();
        getline(cin, airportName);

        string airportNameLower = airportName;
        transform(airportNameLower.begin(), airportNameLower.end(), airportNameLower.begin(), ::tolower);

        for (const auto& flight : flights) {  // filter flights by airport
            string flightAirportNameLower = flight.airport_name;
            transform(flightAirportNameLower.begin(), flightAirportNameLower.end(), flightAirportNameLower.begin(), ::tolower);

            if (flightAirportNameLower.find(airportNameLower) != string::npos) {
                selectedFlights.push_back(flight);
            }
        }

        if (selectedFlights.empty()) {
            cout << "No flights found for airport city containing: " << airportName << endl;
            return 1;
        }
    }

    if (sortingMethod == 1) {
        cout << "\nYou selected Quick Sort.\n";
    } else if (sortingMethod == 2) {
        cout << "\nYou selected Merge Sort.\n";
    }

    cout << fixed << setprecision(2);

    // best case sorting
    vector<Flight> bestCaseData = getBestCase(selectedFlights);
    auto start = high_resolution_clock::now();

    if (sortingMethod == 1) {
        quickSort(bestCaseData, 0, bestCaseData.size() - 1);
    } else {
        // FIXME: need to implement merge sort call here
        // call mergeSort for best case data
        mergeSort(bestCaseData, 0, bestCaseData.size() - 1);
    }

    auto end = high_resolution_clock::now();
    auto durationBest = duration<double, milli>(end - start);
    cout << "\nBest Case (Already Sorted) Sorting Time: " << durationBest.count() << " ms" << endl;

    if (!bestCaseData.empty()) {  // output delays for best case
        cout << "Shortest delay: " << bestCaseData.front().arr_delay << " minutes" << endl;
        cout << "Longest delay: " << bestCaseData.back().arr_delay << " minutes" << endl;
    }

    // worst case sorting
    vector<Flight> worstCaseData = getWorstCase(selectedFlights);
    start = high_resolution_clock::now();

    if (sortingMethod == 1) {
        quickSort(worstCaseData, 0, worstCaseData.size() - 1);
    } else {
        // FIXME: implement merge sort call
        // Call mergeSort for worst case data
        mergeSort(worstCaseData, 0, worstCaseData.size() - 1);
    }

    end = high_resolution_clock::now();
    auto durationWorst = duration<double, milli>(end - start);
    cout << "\nWorst Case (Reverse Sorted) Sorting Time: " << durationWorst.count() << " ms" << endl;

    if (!worstCaseData.empty()) {  // output delays for worst case
        cout << "Shortest delay: " << worstCaseData.front().arr_delay << " minutes" << endl;
        cout << "Longest delay: " << worstCaseData.back().arr_delay << " minutes" << endl;
    }

    // average case sorting
    vector<Flight> averageCaseData = getAverageCase(selectedFlights);
    start = high_resolution_clock::now();

    if (sortingMethod == 1) {
        quickSort(averageCaseData, 0, averageCaseData.size() - 1);
    } else {
        // FIXME: implement merge sort call
        // call mergeSort for average case data
        mergeSort(averageCaseData, 0, averageCaseData.size() - 1);
    }

    end = high_resolution_clock::now();
    auto durationAverage = duration<double, milli>(end - start);
    cout << "\nAverage Case (Random Order) Sorting Time: " << durationAverage.count() << " ms" << endl;

    if (!averageCaseData.empty()) {  // output delays for average case
        cout << "Shortest delay: " << averageCaseData.front().arr_delay << " minutes" << endl;
        cout << "Longest delay: " << averageCaseData.back().arr_delay << " minutes" << endl;
    }

    return 0;
}
