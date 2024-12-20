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
    int maxEntries = int(BLOCK_SIZE / maxObjectSize);
    newRStarTree *rStarTree = new newRStarTree(maxEntries, dimensions, maxObjectSize);
    unsigned int blockCount = 0;
    unsigned int pointCount = 0;
    unsigned int totalPoints = 0;
    unsigned int slot = 0;
    int pointsPerBlock = 0;
    string line;
    bool sortFirst = true;

    // Transform the map file to a csv file that has the format: id, name, lat, lon, ...
    writeToCSV(CSV_FILE, MAP_FILE, attributeNames, totalPoints, sortFirst);

    int numWays = (totalPoints + maxEntries - 1) / maxEntries;

    // Write the data of the .csv file into blocks
    fstream dataFile(DATA_FILE, ios::out);
    fstream csvFile(CSV_FILE);
    fstream sortedCsvFile(SORTED_CSV_FILE);
    fstream indexFile(INDEX_FILE);

    externalSort(CSV_FILE, SORTED_CSV_FILE, numWays, maxObjectSize);
    cout << "Sorting completed." << endl;

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

    pointsPerBlock = int(BLOCK_SIZE / maxObjectSize);
    dataFile << "BLOCK" << blockCount++ << endl;
    dataFile << "block size:" << BLOCK_SIZE << endl;
    dataFile << "points:" << totalPoints << endl;
    dataFile << "points/block:" << pointsPerBlock << endl;
    dataFile << "dimensions:" << dimensions << endl;
    dataFile << "capacity:" << maxEntries << endl;

    // Read each line of the file and parse it into a Point structure
    auto startTime = chrono::high_resolution_clock::now();
    while (getline(sortedCsvFile, line)) {
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

    //  cout << "Leaves of the original R* Tree:" << endl;
    //  rStarTree->display();

    vector<Node*> leafNodes = {};
    Node *currentLeafNode = new Node(dimensions, true);
    leafNodes.push_back(currentLeafNode);
    while (getline(sortedCsvFile, line)) {
        Point point = parsePoint(line);
        string record = point.toString();

        if (currentBlockSize + maxObjectSize > BLOCK_SIZE) {
            leafNodes.push_back(currentLeafNode);
            currentLeafNode = new Node(dimensions, true);
            currentBlockSize = 0;
            slot = 0;
            dataFile << "BLOCK" << ++blockCount << endl;
        }

        // Write point into datafile and insert it to the tree
        dataFile << record << endl;

        Entry *entry = new Entry();
        entry->childNode = nullptr;
        entry->boundingBox = new BoundingBox(dimensions, point.getCoordinates(), point.getCoordinates());
        entry->id = new ID();
        entry->id->blockID = blockCount;
        entry->id->slot = slot;
        currentLeafNode->insertEntry(entry);
        currentBlockSize += maxObjectSize;

        slot++;
    }

    cout << "bottom up" << endl;

    // Bottom-up construction
    vector<Node*> levelNodes = leafNodes;
    int level = 0;
    while (levelNodes.size() > 1) {
        vector<Node*> newLevelNodes;
//        int numLevelNodes = static_cast<int>(ceil(static_cast<double>(levelNodes.size()) / maxEntries));
        int numLevelNodes = ceil(levelNodes.size() / maxEntries);
        cout << numLevelNodes << endl;

        for (int i = 0; i < numLevelNodes; i++) {
            Node* newNode = new Node(dimensions, false);
            newNode->setLevel(level + 1);

            int startIndex = i * maxEntries;
            int endIndex = min(startIndex + maxEntries, static_cast<int>(levelNodes.size()));
            for (int j = startIndex; j < endIndex; j++) {
                Entry* entry = new Entry();
                entry->childNode = levelNodes[j];
                entry->boundingBox = levelNodes[j]->adjustBoundingBoxes();
                newNode->insertEntry(entry);
            }

            newLevelNodes.push_back(newNode);
        }

        level++;
        levelNodes = newLevelNodes;
    }

    // The root node is the last remaining node in the level nodes
    cout << "bottom up" << endl;
    newRStarTree *rStarTreeFromIndex = new newRStarTree(maxEntries, dimensions, maxObjectSize);
    rStarTreeFromIndex->setRoot(leafNodes[0]);

    indexFile.close();
    dataFile.close();
    csvFile.close();

    return 0;
}