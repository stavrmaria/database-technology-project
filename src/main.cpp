#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

using namespace std;

#include "Point.h"
#include "Node.h"
#include "BoundingBox.h"
#include "RStarTree.h"
#include "constants.h"

#include "RStarTree.h"

int main() {
    // Set the points attributes
    int dimensions = 2;
    int maxObjectSize = dimensions * sizeof(double) + 100 * sizeof(char);
    int maxEntries = BLOCK_SIZE / maxObjectSize;
    maxEntries = 3;
    newRStarTree *rStarTree = new newRStarTree(maxEntries, dimensions, maxObjectSize);
    unsigned int blockCount = 0;
    unsigned int pointCount = 0;
    unsigned int slot = 0;
    unsigned int currentBlockSize = 0;
    int pointsPerBlock = 0;
    string line;

    // Tranform the map file to a csv file that has
    // the format: id, lat, lon, ...
    writeToCSV(CSV_FILE, MAP_FILE, attributeNames, pointCount);

    // Write the data of the .csv file into blocks
    fstream dataFile(DATA_FILE, ios::out);
    ifstream csvFile(CSV_FILE);
    ofstream indexFile(INDEX_FILE);
    cout << maxObjectSize << endl;

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

    pointsPerBlock = int(BLOCK_SIZE / maxObjectSize);
    dataFile << "BLOCK" << blockCount++ << endl;
    dataFile << "block size:" << BLOCK_SIZE << endl;
    dataFile << "points:" << pointCount << endl;
    dataFile << "points/block:" << pointsPerBlock << endl;
    dataFile << "dimensions:" << dimensions << endl;
    dataFile << "capacity:" << maxEntries << endl;
    dataFile << "BLOCK" << blockCount << endl;
    // Read each line of the file and parse it into a Point structure
    while (getline(csvFile, line)) {
        Point point = parsePoint(line);
        string record = point.toString();

        if (currentBlockSize + maxObjectSize > BLOCK_SIZE) {
            currentBlockSize = 0;
            slot = 0;
            dataFile << "BLOCK" << ++blockCount << endl;
        }

        // Write point into datafile and insert it to the tree
        dataFile << record << endl;
        rStarTree->insertData(point, blockCount, slot);
        slot++;
        currentBlockSize += maxObjectSize;
    }

    // Save the R* tree index to the index file and the data file
    if (rStarTree->saveIndex(INDEX_FILE) == 1)
        return 1;
    
    cout << "========== R* ==========\n";
    rStarTree->display();
    
    indexFile.close();
    dataFile.close();
    csvFile.close();

    return 0;
}