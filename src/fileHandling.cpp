#include "RStarTree.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "pugixml.hpp"

using namespace std;

// Save R* Tree into the indexfile
int newRStarTree::saveIndex(const string &indexFileName) {
    fstream indexfile;
    indexfile.open(indexFileName, ios::out | ios::binary);

    if (!indexfile) {
        cerr << "Error: could not create " << indexFileName << endl;
        return 1;
    }

    cout << "Saving index to " << indexFileName << "...\n";
    saveIndex(indexfile, this->root);
    indexfile.close();
    cout << "Saving completed successfully." << endl;

    return 0;
}

// Save the leaves of the R* Tree by traversing the nodes with the DFS algorithm
void newRStarTree::saveIndex(fstream& indexFile, Node* currentNode) {
    if (currentNode == nullptr) {
        return;
    }

    // Save the basic attributes of the R* Tree
    indexFile.write(reinterpret_cast<char*>(&dimensions), sizeof(int));
    indexFile.write(reinterpret_cast<char*>(&minEntries), sizeof(int));
    indexFile.write(reinterpret_cast<char*>(&maxEntries), sizeof(int));
    indexFile.write(reinterpret_cast<char*>(&maxObjectSize), sizeof(int));

    queue<Node*> nodeQueue;
    nodeQueue.push(currentNode);
    while (!nodeQueue.empty()) {
        currentNode = nodeQueue.front();
        nodeQueue.pop();

        // Serialize and save the current node in the index file
        currentNode->serializeNode(indexFile);

        // Push child nodes onto the queue
        if (!currentNode->isLeafNode()) {
            for (auto& child : currentNode->getEntries()) {
                nodeQueue.push(child->childNode);
            }
        }
    }
}

int newRStarTree::saveData(int dimensions,const string &dataFileName) {
    fstream dataFile;
    dataFile.open(dataFileName, ios::out | ios::binary);

    if (!dataFile) {
        cerr << "Error: could not create " << dataFileName << endl;
        return 1;
    }

    cout << "Saving data to " << dataFileName << "...\n";
    saveData(dimensions, dataFile, this->root);
    dataFile.close();
    cout << "Saving completed successfully." << endl;

    return 0;
}

void newRStarTree::saveData(int  dimensions, fstream& dataFile, Node *currentNode) {
    if (currentNode == nullptr) {
        return;
    }

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
 
    pointsPerBlock = int(BLOCK_SIZE / maxObjectSize);
    dataFile << "BLOCK" << blockCount++ << endl;
    dataFile << "block size:" << BLOCK_SIZE << endl;
    dataFile << "points:" << totalPoints << endl;
    dataFile << "points/block:" << pointsPerBlock << endl;
    dataFile << "dimensions:" << dimensions << endl;
    dataFile << "capacity:" << maxEntries << endl;

    unsigned long blockID = 0;
    dataFile << "BLOCK" << to_string(blockID++) << endl;
    dataFile << "nodes:" << this->nodesCount << endl;

    stack<Node*> nodeStack;
    nodeStack.push(currentNode);
    while (!nodeStack.empty()) {
        Node *current = nodeStack.top();
        nodeStack.pop();

        if (current->isLeafNode()) {
            dataFile << "BLOCK" << to_string(blockID++) << endl;
            dataFile << current->getEntries().size() << endl;
            for (auto entry : current->getEntries()) {
                Point point = findObjectById(*(entry->id), maxObjectSize);
                dataFile << point << endl;
            }
        }

        // Push child nodes onto the stack
        for (auto entry : current->getEntries()) {
            if (entry->childNode == nullptr)
                continue;
            nodeStack.push(entry->childNode);
        }
    }
}

// Parse a line and create a point based on it's attributes
Point parsePoint(string line) {
    Point point;
    vector<string> row;
    stringstream ss(line);
    string cell;

    while (getline(ss, cell, ',')) {
        row.push_back(cell);
    }

    point.setID(stoull(row.at(0)));
    if (row.at(1) != "")
        point.setName(row.at(1));

    for (int i = 2; i < row.size(); i++)
        point.addDimension(stod(row.at(i)));

    row.clear();
    return point;
}

// Find the point based on it's location on the datafile
Point findObjectById(ID id, int &pointsPerBlock) {
    ifstream datafile(DATA_FILE);
    unsigned int lineIndex = 1;
    unsigned int currentLine = 1;
    string line;

    // Read each line of the file unit you find the first block
    while (getline(datafile, line)) {
        if (line.find("BLOCK1") != string::npos)
            break;
        lineIndex++;
    }
    lineIndex++;

    // Calculate the line index of the desired point
    currentLine = lineIndex;
    lineIndex += (id.blockID - 1) * (pointsPerBlock + 1) + id.slot;
    while (getline(datafile, line)) {
        if (currentLine == lineIndex) {
            break;
        }
        currentLine++;
    }

    datafile.close();
    return parsePoint(line);
}

// Convert the .osm file into a .csv file based on it's the attributes
void writeToCSV(const string& csvFileName, const string& mapFileName, const vector<string>& attributeNames, unsigned int &pointCount, bool &sortFirst) {
    ofstream csvFile(csvFileName);
    pugi::xml_document doc;

    if (doc.load_file(mapFileName.c_str())) {
        vector<vector<string>> csvData;
        // Traverse the XML and extract the desired data
        for (const auto& node : doc.select_nodes("//node")) {
            vector<string> rowData;

            for (const auto& attributeName : attributeNames) {
                rowData.push_back(node.node().attribute(attributeName.c_str()).value());
            }

            csvData.push_back(rowData);
            rowData.clear();
            pointCount++;
        }

        // Write the data to the CSV file
        for (const auto& row : csvData) {
            for (size_t i = 0; i < row.size(); ++i) {
                csvFile << row[i];
                if (i != row.size() - 1) {
                    csvFile << ",";
                }
            }

            csvFile << endl;
        }

        cout << "CSV file created successfully." << endl;
        csvData.clear();
    } else {
        cout << "Failed to load XML file." << endl;
    }

    csvFile.close();
}

newRStarTree* constructFromIndex(const string& indexFileName) {
    cout << "Constructing the R* Tree from the " << indexFileName << "..." << endl;

    ifstream indexFile(indexFileName, ios::binary);
    if (!indexFile) {
        cerr << "Failed to open the index file." << endl;
        return nullptr;
    }

    // Get the basic attributes of the R* Tree
    int dimensions;
    int minEntries;
    int maxEntries;
    int maxObjectSize;
    indexFile.read(reinterpret_cast<char*>(&dimensions), sizeof(int));
    indexFile.read(reinterpret_cast<char*>(&minEntries), sizeof(int));
    indexFile.read(reinterpret_cast<char*>(&maxEntries), sizeof(int));
    indexFile.read(reinterpret_cast<char*>(&maxObjectSize), sizeof(int));

    cout << "Collected the basic attributes..." << endl;

    newRStarTree* tree = new newRStarTree(maxEntries, dimensions, maxObjectSize);
    Node* root = new Node(dimensions, false);
    root->deserializeNode(indexFile);

    stack<Node*> nodeStack;
    nodeStack.push(root);
    while (!nodeStack.empty()) {
        Node* current = nodeStack.top();
        nodeStack.pop();

        for (const auto &entry : current->getEntries()) {
            if (current->getLevel() < 1)
                continue;
            int isLeaf = (current->getLevel() == 1);
            entry->childNode = new Node(dimensions, isLeaf);
            entry->childNode->setParent(current);
            entry->childNode->deserializeNode(indexFile);
            if (!entry->childNode->isLeafNode())
                nodeStack.push(entry->childNode);
        }
    }

    tree->setRoot(root);
    indexFile.close();
    cout << "Construction completed." << endl;

    return tree;
}
