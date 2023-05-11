#include "RStarTree.h"

// Save R* Tree into the indexfile
int RStarTree::saveIndex(const string &indexFileName) {
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
void RStarTree::saveIndex(fstream& indexFile, Node *currentNode) {
    if (currentNode == nullptr) {
        return;
    }

    stack<Node*> nodeStack;
    nodeStack.push(currentNode);
    while (!nodeStack.empty()) {
        Node currentNode = *(nodeStack.top());
        nodeStack.pop();

        // Serialize and save the  current node in the index file
        indexFile.write((char*)&currentNode, sizeof(currentNode));
        // Push child nodes onto the stack
        for (auto entry : currentNode.getEntries()) {
            if (entry->childNode == nullptr)
                continue;
            nodeStack.push(entry->childNode);
        }
    }
}

// Parse a line and create a point based on it's attributes
Point parsePoint(string line) {
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
                if (attributeName == "id" && point.getID() == 0)
                    point.setID(stoul(attributeValue));
                else if (attributeName == "user" && point.getName() == "-")
                    point.setName(attributeValue);
                else if (attributeName != "id" && attributeName != "name")
                    point.addDimension(stod(attributeValue));
            }
        }
    }
    
    return point;
}

Point findObjectById(unsigned long &id) {
    ifstream mapFile(MAP_FILE);
    string line;
    bool afterBounds = false;

    // Read each line of the file and parse it into a Point structure
    while (getline(mapFile, line)) {
        if (line.find("<node id=") == std::string::npos)
            continue;

        // Extract the ID from the line
        size_t startPos = line.find("\"") + 1;
        size_t endPos = line.find("\"", startPos);
        unsigned long currentID = stoul(line.substr(startPos, endPos - startPos));

        // Check if the ID matches the target ID
        if (id == currentID)
            return parsePoint(line);
    }

    return Point();
}

int RStarTree::saveData(const string &dataFileName) {
    fstream dataFile;
    dataFile.open(dataFileName, ios::out | ios::binary);
    
    if (!dataFile) {
        cerr << "Error: could not create " << dataFileName << endl;
        return 1;
    }

    cout << "Saving data to " << dataFileName << "...\n";
    saveData(dataFile, this->root);
    dataFile.close();
    cout << "Saving completed successfully." << endl;

    return 0;
}

void RStarTree::saveData(fstream& dataFile, Node *currentNode) {
    if (currentNode == nullptr) {
        return;
    }

    unsigned long blockID = 0;
    dataFile << "BLOCKID" << to_string(blockID++) << endl;
    dataFile << "nodes:" << this->nodesCount << endl;

    stack<Node*> nodeStack;
    nodeStack.push(currentNode);
    while (!nodeStack.empty()) {
        Node *current = nodeStack.top();
        nodeStack.pop();

        if (current->isLeafNode()) {
            dataFile << "BLOCKID" << to_string(blockID++) << endl;
            dataFile << current->getEntries().size() << endl;
            for (auto entry : current->getEntries()) {
                Point point = findObjectById(entry->id);
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
