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
    unsigned int slot = 0;
    unsigned int currentBlockSize = 0;
    int pointsPerBlock = 0;
    unsigned int totalPoints=0;
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
    dataFile << "points:" << pointCount << endl;
    dataFile << "points/block:" << pointsPerBlock << endl;
    dataFile << "dimensions:" << dimensions << endl;
    dataFile << "capacity:" << maxEntries << endl;
    dataFile << "BLOCK" << blockCount << endl;

    // Read each line of the file and parse it into a Point structure
    auto startTime = chrono::high_resolution_clock::now();
    while (getline(sortedCsvFile, line)) {
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
    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::micro> duration = endTime - startTime;

    cout << "Insertion completed." << endl;
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    //auto startTime2 = chrono::high_resolution_clock::now();
    BoundingBox bb(dimensions, vector<double>{4, 2}, vector<double> {8, 5});
    vector<ID> res = rStarTree->rangeQuery(bb);
    for (int i = 0; i < res.size(); i++)
        cout << findObjectById(res.at(i), pointsPerBlock).getID() << endl;

    // auto endTime2 = chrono::high_resolution_clock::now();
    // chrono::duration<double, std::micro> duration2 = endTime - startTime;    

    cout<<"-----"<<endl;

    Point queryPoint(vector<double>{4,1});
    vector<ID> kres = rStarTree->kNearestNeighbors(queryPoint, 2);
    for (int i = 0; i < kres.size(); i++)
        cout << findObjectById(kres.at(i), pointsPerBlock).getID() << endl;

    cout<<"-----"<<endl;

    vector<ID> sres = rStarTree->skylineQuery();
    for(int i = 0; i < sres.size(); i++)
        cout << findObjectById(sres.at(i), pointsPerBlock).getID() << endl;
    cout<<"-----"<<endl;

    Point deleteP({1, 1});
    rStarTree->deletePoint(deleteP);

    /*cout << "bottom up" << endl;

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

    // Bottom-up construction
    vector<Node*> levelNodes = leafNodes;
    int level = 0;
    while (levelNodes.size() > 1) {
        vector<Node*> newLevelNodes;
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
    rStarTreeFromIndex->setRoot(leafNodes[0]);*/

    if (rStarTree->saveIndex(INDEX_FILE) == 1) {
        cout << "Error: construction of the index was unsuccessful." << endl;
        return 1;
    }

    indexFile.close();
    dataFile.close();
    csvFile.close();

    return 0;
}