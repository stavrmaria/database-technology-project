#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>

#include "constants.h"
#include "Point.h"
#include "RStarTree.h"

using namespace std;

// Parse a line and create a point based on it's attributes
Point parsePoint(string line, vector<string> attributeNames) {
    stringstream ss(line);
    string token;
    Point point;

    while (getline(ss, token, ' ')) {
        for (const auto& attributeName : attributeNames) {
            if (token.find(attributeName + "=") != string::npos) {
                // Get the value of this attribute
                size_t start = token.find('"') + 1;
                size_t end = token.find('"', start);
                string attributeValue = token.substr(start, end - start);

                // Add the value to the point based on it's type
                if (attributeName == "id" && point.getID() == "-")
                    point.setID(attributeValue);
                else if (attributeName == "user" && point.getName() == "-")
                    point.setName(attributeValue);
                else if (attributeName != "id" && attributeName != "name")
                    point.addDimension(stod(attributeValue));
            }
        }
    }
    
    return point;
}

int main() {
    // Set the points attributes
    vector<string> attributeNames = {"id", "lat", "lon","user"};
    // RStarTree *rStarTree = new RStarTree();

    // Initialize the files
    ofstream dataFile(DATA_FILE);
    filesystem::path filePath = std::filesystem::current_path().parent_path() / MAP_FILE;    
    ofstream indexfile(INDEX_FILE, ios::binary | ios::trunc);
    ifstream mapFile(MAP_FILE);

    // Write points of the .osm file into the datafile and add them to the R-Star Tree
    long long int blockCount = 0;
    int currentBlockSize = 0;
    string line;
    bool afterBounds = false;

    if (!dataFile.is_open()) {
        cerr << "Error: could not open file " << DATA_FILE << endl;
        return 1;
    }

    if (!mapFile.is_open()) {
        cerr << "Error: could not open file " << MAP_FILE << endl;
        return 1;
    }

    if (!indexfile.is_open()) {
        cerr << "Error: could not open file " << INDEX_FILE << endl;
        return 1;
    }

    // Read each line of the file and parse it into a Point structure
    dataFile << "BLOCK " << blockCount++ << endl;
    while (getline(mapFile, line)) {
        if (!afterBounds) {
            // check if we've reached the <bounds> tag
            if (line.find("<bounds") != string::npos)
                afterBounds = true;
            continue;
        }

        // Node found parse it to create a new point
        if (line.find("<node ") != string::npos) {
            Point point = parsePoint(line, attributeNames);

            string record = point.toString();
            int recordSize = record.length();

            if (currentBlockSize + recordSize > BLOCK_SIZE) {
                currentBlockSize = 0;
                dataFile << "BLOCK " << blockCount++ << endl;
            }

            // Write point into datafile
            dataFile << record;
            
            // Add the point in the R* Tree
            // rStarTree->insert(point);

            currentBlockSize += recordSize;
        }
    }

    // Save the R* tree index to the index file

    mapFile.close();
    dataFile.close();
    indexfile.close();

    return 0;
}