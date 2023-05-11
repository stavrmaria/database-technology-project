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

int main() {
    // Set the points attributes
    int dimensions = 2;
    int maxObjectSize = dimensions * sizeof(double) + 100 * sizeof(char);
    int maxEntries = BLOCK_SIZE / maxObjectSize;
    maxEntries = 3;
    RStarTree *rStarTree = new RStarTree(maxEntries, dimensions);

    // Initialize the files
    ofstream dataFile(DATA_FILE);
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
            Point point = parsePoint(line);
            // Add the point in the R* Tree
            rStarTree->insert(point);
        }
    }

    // rStarTree->traverse();

    // Save the R* tree index to the index file and the data file
    if (rStarTree->saveIndex(INDEX_FILE) == 1)
        return 1;
    if (rStarTree->saveData(DATA_FILE) == 1)
        return 1;

    mapFile.close();
    return 0;
}