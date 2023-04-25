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
    int dimensions = 2;
    int maxObjectSize = dimensions * sizeof(double) + 100 * sizeof(char);
    int maxEntries = BLOCK_SIZE / maxObjectSize;
    RStarTree *rStarTree = new RStarTree(maxEntries, dimensions);

    // Initialize the files
    ofstream dataFile(DATA_FILE);
    filesystem::path filePath = std::filesystem::current_path().parent_path() / MAP_FILE;    
    ofstream indexfile(INDEX_FILE, ios::binary | ios::trunc);
    ifstream mapFile(MAP_FILE);

    // Write points of the .osm file into the datafile and add them to the R-Star Tree
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
            // Add the point in the R* Tree
            rStarTree->insert(point);
        }
    }

    rStarTree->traverse();

    // Save the R* tree index to the index file
    // Save the R* tree data to the data file

    mapFile.close();
    dataFile.close();
    indexfile.close();

    return 0;
}