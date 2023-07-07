// #include <iostream>
// #include <fstream>
// #include <string>
// #include <vector>
// #include <sstream>
// #include <chrono>

// using namespace std;

// #include "Point.h"
// #include "RStarTree.h"
// #include "constants.h"

// string doubleToBinaryString(double);
// string zOrderValue(vector<double>);
// newRStarTree *constructFromIndex(const string &indexFileName);

// int insertAction(int, vector<string>);
// int bottomUpAction(int, vector<string>);
// void DeleteAction();
// void rangeQueryAction();
// void skylineQueryAction();
// void knnQueryAction(int);
// void getInput(int *, vector<string> *);

// int main() {
//     int n;
//     int k;
//     char action;
//     vector<string> a = {"id", "name"};

//     cout << "===============================================" << endl;
//     cout << "Welcome to the R*Tree implementation" << endl;

//     do {
//         cout << "====================Options=====================" << endl;
//         cout << "1. Insert from .osm file." << endl;
//         cout << "2. Bottom up from .osm file." << endl;
//         cout << "3. Delete from indexfile." << endl;
//         cout << "4. Range Query from indexfile." << endl;
//         cout << "5. Skyline from indexfile." << endl;
//         cout << "6. Knn from indexfile." << endl;
//         // cout << "7. Show full statistics for all actions from .osm file." << endl;
//         cout << "Choose your action (press q to quit): ";
//         cin >> action;
//         switch (action) {
//             case '1':
//                 getInput(&n, &a);
//                 insertAction(n, a);
//                 break;
//             case '2':
//                 getInput(&n, &a);
//                 bottomUpAction(n, a);
//                 break;
//             case '3':
//                 DeleteAction();
//                 break;
//             case '4':
//                 rangeQueryAction();
//                 break;
//             case '5':
//                 skylineQueryAction();
//                 break;
//             case '6':
//                 cout << "Enter the number k: ";
//                 cin >> k;
//                 knnQueryAction(k);
//                 break;
//             default:
//                 break;
//         }
//     } while (action != 'q');
    
//     cout << "Quit." << endl;

//     return 0;
// }

// void getInput(int *n, vector<string> *attributeNames) {
//     cout << "Enter the no. of dimensions: ";
//     cin >> *n;
//     cout << "Enter the attribute names of the " << *n << " dimensions: ";
//     for (int i = 0; i < *n; i++) {
//         string attributeName;
//         cin >> attributeName;
//         attributeNames->push_back(attributeName);
//     }

//     cout << "Initialization Completed." << endl;
// }

// int insertAction(int dimensions, vector<string> attributeNames) {
//     cout << "==> Insert" << endl;
//     // Set the points attributes
//     int maxObjectSize = dimensions * sizeof(double) + 100 * sizeof(char);
//     int maxEntries = int(BLOCK_SIZE / maxObjectSize);
//     newRStarTree *rStarTree = new newRStarTree(maxEntries, dimensions, maxObjectSize);
//     unsigned int blockCount = 0;
//     unsigned int pointCount = 0;
//     unsigned int totalPoints = 0;
//     unsigned int slot = 0;
//     int pointsPerBlock = 0;
//     string line;
//     bool sortFirst = false;

//     // Transform the map file to a csv file that has the format: id, name, lat, lon, ...
//     writeToCSV(CSV_FILE, MAP_FILE, attributeNames, totalPoints, sortFirst);

//     // Write the data of the .csv file into blocks
//     fstream dataFile(DATA_FILE, ios::out);
//     fstream csvFile(CSV_FILE);
//     fstream indexFile(INDEX_FILE);

//     if (!dataFile.is_open()) {
//         cerr << "Error: could not open file " << DATA_FILE << endl;
//         return 1;
//     }

//     if (!csvFile.is_open()) {
//         cerr << "Error: could not open file " << CSV_FILE << endl;
//         return 1;
//     }

//     if (!indexFile.is_open()) {
//         cerr << "Error: could not open file " << INDEX_FILE << endl;
//         return 1;
//     }

//     cout << "Inserting points..." << endl;

//     pointsPerBlock = int(BLOCK_SIZE / maxObjectSize);
//     dataFile << "BLOCK" << blockCount++ << endl;
//     dataFile << "block size:" << BLOCK_SIZE << endl;
//     dataFile << "points:" << totalPoints << endl;
//     dataFile << "points/block:" << pointsPerBlock << endl;
//     dataFile << "dimensions:" << dimensions << endl;
//     dataFile << "capacity:" << maxEntries << endl;

//     // Read each line of the file and parse it into a Point structure
//     auto startTime = chrono::high_resolution_clock::now();
    
//     while (getline(csvFile, line)) {
//         Point point = parsePoint(line);
//         string record = point.toString();

//         if (pointCount % pointsPerBlock == 0) {
//             slot = 0;
//             dataFile << "BLOCK" << blockCount++ << endl;
//         }

//         // Write point into datafile and insert it to the tree
//         dataFile << record << endl;
//         unsigned int blockID = blockCount - 1;
//         rStarTree->insertData(point, blockID, slot);
//         slot++;
//         pointCount++;
//     }

//     auto endTime = chrono::high_resolution_clock::now();
//     chrono::duration<double, std::micro> duration = endTime - startTime;
//     cout << "Insertion completed." << endl;
//     cout << "Execution time: " << duration.count() << " milliseconds" << endl;

//     // Save the R* tree index to the index file and the data file
//     if (rStarTree->saveIndex(INDEX_FILE) == 1) {
//         cout << "Error: construction of the index was unsuccessful." << endl;
//         return 1;
//     }

//     indexFile.close();
//     dataFile.close();
//     csvFile.close();
//     return 0;
// }

// int bottomUpAction(int dimensions, vector<string> attributeNames) {
//     cout << "==> Bottom Up" << endl;
//     int maxObjectSize = dimensions * sizeof(double) + 100 * sizeof(char);
//     int maxEntries = int(BLOCK_SIZE / maxObjectSize);
//     newRStarTree *rStarTree = new newRStarTree(maxEntries, dimensions, maxObjectSize);
//     unsigned int blockCount = 0;
//     unsigned int pointCount = 0;
//     unsigned int totalPoints = 0;
//     unsigned int slot = 0;
//     int pointsPerBlock = 0;
//     string line;
//     bool sortFirst = true;

//     // Transform the map file to a csv file that has the format: id, name, lat, lon, ...
//     writeToCSV(CSV_FILE, MAP_FILE, attributeNames, totalPoints, sortFirst);

//     // Write the data of the .csv file into blocks
//     fstream dataFile(DATA_FILE, ios::out);
//     fstream csvFile(CSV_FILE);
//     fstream sortedCsvFile(SORTED_CSV_FILE);
//     fstream indexFile(INDEX_FILE);

//     int numWays = (totalPoints + maxEntries - 1) / maxEntries;
//     externalSort(CSV_FILE, SORTED_CSV_FILE, numWays, maxObjectSize);
//     cout << "Sorting completed." << endl;

//     if (!dataFile.is_open()) {
//         cerr << "Error: could not open file " << DATA_FILE << endl;
//         return 1;
//     }

//     if (!csvFile.is_open()) {
//         cerr << "Error: could not open file " << CSV_FILE << endl;
//         return 1;
//     }

//     if (!indexFile.is_open()) {
//         cerr << "Error: could not open file " << INDEX_FILE << endl;
//         return 1;
//     }

//     cout << "Inserting points..." << endl;

//     pointsPerBlock = int(BLOCK_SIZE / maxObjectSize);
//     dataFile << "BLOCK" << blockCount++ << endl;
//     dataFile << "block size:" << BLOCK_SIZE << endl;
//     dataFile << "points:" << totalPoints << endl;
//     dataFile << "points/block:" << pointsPerBlock << endl;
//     dataFile << "dimensions:" << dimensions << endl;
//     dataFile << "capacity:" << maxEntries << endl;

//     /*BULK LOADING*/
//     // Read each line of the file and parse it into a Point structure
//     auto startTime = chrono::high_resolution_clock::now();
//     while (getline(sortedCsvFile, line)) {
//         Point point = parsePoint(line);
//         string record = point.toString();

//         if (pointCount % pointsPerBlock == 0) {
//             slot = 0;
//             dataFile << "BLOCK" << blockCount++ << endl;
//         }

//         // Write point into datafile and insert it to the tree
//         dataFile << record << endl;
//         unsigned int blockID = blockCount - 1;
//         rStarTree->insertData(point, blockID, slot);
//         slot++;
//         pointCount++;
//     }
//     auto endTime = chrono::high_resolution_clock::now();
//     chrono::duration<double, std::micro> duration = endTime - startTime;

//     cout << "Insertion completed." << endl;
//     cout << "Execution time: " << duration.count() << " milliseconds" << endl;

//     // Save the R* tree index to the index file and the data file
//     if (rStarTree->saveIndex(INDEX_FILE) == 1) {
//         cout << "Error: construction of the index was unsuccessful." << endl;
//         return 1;
//     }

//     indexFile.close();
//     dataFile.close();
//     csvFile.close();
//     return 0;
// }

// void DeleteAction() {
//     cout << "==> Delete" << endl;
//     newRStarTree *rStarTree = nullptr;

//     if (rStarTree == nullptr)
//         return;

//     vector<double> coordinates = {};
//     int n = rStarTree->getDimensions();
//     cout << "Enter the " << n << " coordinates: " << endl;
//     for (int i = 0; i < n; i++) {
//         double coordinate;
//         cin >> coordinate;
//         coordinates.push_back(coordinate);
//     }

//     Point pointToDelete(coordinates);
//     rStarTree->deletePoint(pointToDelete);
// }

// void skylineQueryAction() {
//     cout << "==> Range Query" << endl;
// }

// void rangeQueryAction() {
//     cout << "==> Skyline Query" << endl;
// }

// void knnQueryAction(int k) {
//     cout << "==> Knn Query" << endl;
// }