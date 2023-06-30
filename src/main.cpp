#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <chrono>

using namespace std;

#include "Point.h"
#include "RStarTree.h"
#include "constants.h"

string doubleToBinaryString(double);
string zOrderValue(vector<double>);
newRStarTree *constructFromIndex(const string &indexFileName);

int main() {
    // Set the points attributes
    int dimensions = 2;
    int maxObjectSize = dimensions * sizeof(double) + 100 * sizeof(char);
    int maxEntries = 3;//int(BLOCK_SIZE / maxObjectSize);
    newRStarTree *rStarTree = new newRStarTree(maxEntries, dimensions, maxObjectSize);
    unsigned int blockCount = 0;
    unsigned int pointCount = 0;
    unsigned int totalPoints = 0;
    unsigned int slot = 0;
    int pointsPerBlock = 0;
    string line;

    // Transform the map file to a csv file that has the format: id, name, lat, lon, ...
    writeToCSV(CSV_FILE, MAP_FILE, attributeNames, totalPoints);

    // Write the data of the .csv file into blocks
    fstream dataFile(DATA_FILE, ios::out);
    ifstream csvFile(CSV_FILE);
    ofstream indexFile(INDEX_FILE);

    if (!dataFile.is_open()) {
        cerr << "Error: could not open file " << DATA_FILE << endl;
        return 1;
    }

    if (!csvFile.is_open()) {
        cerr << "Error: could not open file " << CSV_FILE << endl;
        return 1;
    }

    if (!indexFile.is_open()) {
        cerr << "Error: could not open file " << INDEX_FILE << endl;
        return 1;
    }

    cout << "Inserting points..." << endl;

    pointsPerBlock = 3;// int(BLOCK_SIZE / maxObjectSize);
    dataFile << "BLOCK" << blockCount++ << endl;
    dataFile << "block size:" << BLOCK_SIZE << endl;
    dataFile << "points:" << totalPoints << endl;
    dataFile << "points/block:" << pointsPerBlock << endl;
    dataFile << "dimensions:" << dimensions << endl;
    dataFile << "capacity:" << maxEntries << endl;

    // Read each line of the file and parse it into a Point structure
    auto startTime = chrono::high_resolution_clock::now();
    while (getline(csvFile, line)) {
        Point point = parsePoint(line);
        string record = point.toString();

        if (pointCount % pointsPerBlock == 0) {
            slot = 0;
            dataFile << "BLOCK" << blockCount++ << endl;
        }

        // Write point into datafile and insert it to the tree
        dataFile << record << endl;
        unsigned int blockID = blockCount - 1;
        rStarTree->insertData(point, blockID, slot);
        slot++;
        pointCount++;
    }
    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::micro> duration = endTime - startTime;

    cout << "Insertion completed." << endl;
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    // Save the R* tree index to the index file and the data file
    if (rStarTree->saveIndex(INDEX_FILE) == 1) {
        cout << "Error: construction of the index was unsuccessful." << endl;
        return 1;
    }

     cout << "Leaves of the original R* Tree:" << endl;
     rStarTree->display();

    indexFile.close();
    dataFile.close();
    csvFile.close();

    return 0;
}