#include "newRStarTree.h"

// Basic constructor for the R* Tree
newRStarTree::newRStarTree(int maxEntries, int dimensions, int maxObjectSize) {
    this->dimensions = dimensions;
    this->root = new Node(true);
    this->maxEntries = maxEntries;
    this->minEntries = (int)(maxEntries / 2);
    this->nodesCount = 1;
    this->maxObjectSize = maxObjectSize;
    this->levelCallMap.insert({0, false});
}

// Recursively delete the R* Tree
newRStarTree::~newRStarTree() {
    if (root != nullptr) {
        destroyNode(root);
        delete root;
        root = nullptr;
    }
}

void newRStarTree::destroyNode(Node* currentNode) {
    if (currentNode->isLeafNode()) {
        // Delete leaf node and its entries
        for (auto entry : currentNode->getEntries()) {
            delete entry;
        }
    } else {
        // Recursively destroy child nodes
        for (auto entry : currentNode->getEntries()) {
            destroyNode(entry->childNode);
            delete entry->childNode;
        }
    }
}
// Return the root of the tree
Node *newRStarTree::getRoot() {
    return this->root;
}

// Return the number of nodes of the tree
unsigned long newRStarTree::getNodesCount() const {
    return this->nodesCount;
}

/******************** Insertion Functions ********************/
// Insert a point into the R* Tree
void newRStarTree::insertData(Point &point, unsigned int &blockID, unsigned int &slot) {
    cout << "Inserting Data " << point.getID() << "..." << endl;
    Node *currentNode = this->root;
    
    // Create the entry
    Entry *entry = new Entry();
    entry->childNode = nullptr;
    entry->boundingBox = new BoundingBox(this->dimensions, point.getCoordinates(), point.getCoordinates());
    entry->id = new ID;
    entry->id->blockID = blockID;
    entry->id->slot = slot;

    insert(entry, currentNode);
}

void newRStarTree::insert(Entry *entry, Node *currentNode) {
    cout << "Inserting ..." << endl;
    Node *newNode = nullptr;
    Node *subtreeNode = chooseSubtree(entry, currentNode);
    subtreeNode->insertEntry(entry);

    // There is not available space to place the point
    if (subtreeNode->entriesSize() > this->maxEntries) {
        newNode = new Node(true);
        cout << "new node lvl(insert): " << subtreeNode->getLevel() << endl;
        newNode->setLevel(subtreeNode->getLevel());
        overFlowTreatment(subtreeNode, newNode);
    }

    pair<Node*, Node*> adjustedNodes = adjustTree(subtreeNode, newNode);
    subtreeNode = adjustedNodes.first;
    newNode = adjustedNodes.second;

    // The node split propagation caused the root to split, create a new root
    if (subtreeNode->getParent() == nullptr && newNode != nullptr) {
        cout << "\tNew root" << endl;
        Node *parentNode = new Node(false);
        parentNode->setLevel(0);

        Entry *firstEntry = new Entry();
        firstEntry->childNode = subtreeNode;
        subtreeNode->setParent(parentNode);
        subtreeNode->setLevel(1);
        firstEntry->boundingBox = new BoundingBox(this->dimensions);
        for (auto entry : subtreeNode->getEntries())
            firstEntry->boundingBox->includeBox(*entry->boundingBox);

        Entry *secondEntry = new Entry();
        secondEntry->childNode = newNode;
        newNode->setParent(parentNode);
        newNode->setLevel(1);
        secondEntry->boundingBox = new BoundingBox(this->dimensions);
        for (auto entry : newNode->getEntries())
            secondEntry->boundingBox->includeBox(*entry->boundingBox);
        
        parentNode->insertEntry(firstEntry);
        parentNode->insertEntry(secondEntry);
        currentNode = parentNode;
        this->nodesCount++;
    }

    cout << "Sucessfull insertion." << endl;
    cout << "------------------------------------------" << endl;

    this->root = currentNode;
    this->nodesCount++;
}

// Find the appropriate entry for a given point based on the MBBs
Node* newRStarTree::chooseSubtree(Entry* newEntry, Node* node) {
    Node* currentNode = node;

    // Traverse the tree by selecting the entry whose rectangle needs
    // least enlargement to include the new point
    while (!currentNode->isLeafNode()) {
        Entry* entryToChoose = nullptr;
        vector<Entry*> entries = currentNode->getEntries();

        // The childpointers in the current node point to leaves 
        if (currentNode->getEntries().at(0)->childNode->isLeafNode()) {
            double minOverlapEnlargement;
            double minAreaEnlargement = numeric_limits<double>::infinity();

            // Find the entry in the current node whose rectangle needs least overlap enlargement
            // Resolve ties by choosing the entry whose rectangle needs least area enlargement
            entryToChoose = currentNode->minOverlapEntry(newEntry, &minOverlapEnlargement);
            for (auto entry : entries) {
                BoundingBox originalEntryBB = *(entry->boundingBox);
                BoundingBox updatedEntryBB = *(entry->boundingBox);
                updatedEntryBB.includeBox(*(newEntry->boundingBox));
                double overlapEnlargement = originalEntryBB.calculateOverlap(updatedEntryBB);
                double areaEnlargement = updatedEntryBB.getArea() - originalEntryBB.getArea();
                if (overlapEnlargement == minOverlapEnlargement && areaEnlargement < minAreaEnlargement) {
                    minAreaEnlargement = areaEnlargement;
                    entryToChoose = entry;
                }
            }
        } else {
            // The childpointers in the current node point to leaves, determine the minimum area cost
            // Resolve ties by choosing the entry with the rectangle of smallest area
            double minAreaEnlargement;
            entryToChoose = currentNode->minEnlargedAreaEntry(newEntry, &minAreaEnlargement);
            for (auto entry : entries) {
                BoundingBox originalEntryBB = *(entry->boundingBox);
                BoundingBox updatedEntryBB = *(entry->boundingBox);
                updatedEntryBB.includeBox(*(newEntry->boundingBox));
                double areaEnlargement = updatedEntryBB.getArea() - originalEntryBB.getArea();
                if (areaEnlargement == minAreaEnlargement && entry->boundingBox->getArea() < entryToChoose->boundingBox->getArea()) {
                    minAreaEnlargement = areaEnlargement;
                    entryToChoose = entry;
                }
            }
        }
        
        currentNode = entryToChoose->childNode;
    }
    
    cout << "\tChoose SubTree: ";
    for (int i = 0; i < currentNode->getEntries().size(); i++)
        cout << currentNode->getEntries().at(i)->id->blockID << " " << currentNode->getEntries().at(i)->id->slot << ", ";
    cout << endl;

    return currentNode;
}

void newRStarTree::overFlowTreatment(Node* currentNode, Node *newNode) {
    cout << "\tOverflow treatment" << endl;
    for (auto const &pair: levelCallMap) {
        std::cout << "{" << pair.first << ": " << pair.second << "} ";
    }
    cout << "\n";

    bool isFirstCall = isFirstCallOfLevel(currentNode->getLevel());
    cout << "\t\tisFirstCallOfLevel : " << isFirstCall << endl;

    if (currentNode->getLevel() != 0 && isFirstCall)
        reInsert(currentNode, newNode);
    else
        splitNode(currentNode, newNode);
}

// Check if the overflow treatment has been called before in the node's level
bool newRStarTree::isFirstCallOfLevel(int level) {
    auto it = this->levelCallMap.find(level);
    if (it == this->levelCallMap.end()) {
        this->levelCallMap[level] = false;
    } else {
        this->levelCallMap[level] = true;
    }

    return this->levelCallMap[level];
}

void newRStarTree::reInsert(Node *currentNode, Node *newNode) {
    cout << "\tResinsert" << endl;

    // Calculate for each entry the distance between the entry itself and the current node's MBB
    vector<Entry*> entries = currentNode->getEntries();
    vector<double> distances;
    BoundingBox nodeBoundingBox(this->dimensions);
    for (int i = 0; i < entries.size(); i++)
        nodeBoundingBox.includeBox(*(entries.at(i)->boundingBox));

    Point nodeCenterPoint = nodeBoundingBox.getCenter();
    for (int i = 0; i < entries.size(); i++) {
        Point centerPoint = entries.at(i)->boundingBox->getCenter();
        double distance = nodeCenterPoint.getDistance(centerPoint);
        distances.push_back(distance);
    }

    // Sort the entries based on the decreasing order of the distances 
    sort(entries.begin(), entries.end(), [&](const Entry* x, const Entry* y) {
        int indexX = distance(entries.begin(), find(entries.begin(), entries.end(), x));
        int indexY = distance(entries.begin(), find(entries.begin(), entries.end(), y));
        return distances[indexX] < distances[indexY];
    });

    // Remove the first p entries from the current node
    int removedEntriesNum = int(REINSERTION_PER * (this->maxEntries + 1));
    currentNode->clearEntries();
    for (int i = removedEntriesNum; i < entries.size(); i++)
        currentNode->insertEntry(entries.at(i));

    // Reinsert the removed entries
    for (int i = 0; i < removedEntriesNum; i++)
        insert(entries.at(i),this->root);
}

void newRStarTree::splitNode(Node *currentNode, Node *newNode) {
    cout << "\tSplit" << endl;

    int selectedAxis = chooseSplitAxis(currentNode);
    cout << "\t\t\tselected axis: " << selectedAxis << endl;

    int selectedSplitIndex = chooseSplitIndex(currentNode, selectedAxis);
    cout << "\t\t\tselected split index: " << selectedSplitIndex << endl;
    vector<Entry*> sortedEntries = currentNode->getEntries();
    sort(sortedEntries.begin(), sortedEntries.end(), [&selectedAxis](Entry *a, Entry *b) {
        return a->boundingBox->compareBoundingBox(*(b->boundingBox), selectedAxis);
    });

    for (int i = 0; i < sortedEntries.size(); i++)
        cout << currentNode->getEntries().at(i)->id->blockID << " " << currentNode->getEntries().at(i)->id->slot << ", ";
    cout << endl;

    currentNode->clearEntries();
    newNode->clearEntries();
    for (int i = 0; i < this->minEntries - 1 + selectedSplitIndex; i++)
        currentNode->insertEntry(sortedEntries.at(i));
    for (int i = this->minEntries - 1 + selectedSplitIndex; i < this->maxEntries + 1; i++)
        newNode->insertEntry(sortedEntries.at(i));    
}

// Determme the axis, perpendicular to which the split is performed
int newRStarTree::chooseSplitAxis(Node *currentNode) {
    double minS = numeric_limits<double>::infinity();
    int minSIndex = 0;

    for (int i = 0; i < this->dimensions; i++) {
        // Sort the entries by the lower and upper value of their rectangles for the current axis
        vector<Entry*> sortedEntries = currentNode->getEntries();
        sort(sortedEntries.begin(), sortedEntries.end(), [i](Entry *a, Entry *b) {
            return a->boundingBox->compareBoundingBox(*(b->boundingBox), i);
        });

        // Determine the distributions, where the first group constains m - 1 + k entries
        for (int k = 0; k < this->maxEntries - 2 * this->minEntries + 2; k++) {
            BoundingBox firstGroupBB(this->dimensions);
            BoundingBox secondGroupBB(this->dimensions);

            // Create the two groups of the MBBs
            for (int j = 0; j < this->minEntries - 1 + k; j++)
                firstGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
            for (int j = this->minEntries - 1 + k; j < this->maxEntries + 1; j++)
                secondGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
            
            double marginValue = firstGroupBB.calculateMargin() + secondGroupBB.calculateMargin();
            // Update minimum S and split axis if necessary
            if (marginValue < minS) {
                minS = marginValue;
                minSIndex = i;
            }
        }
    }

    return minSIndex;
}

// Along the chosen split axis, choose the distribution with the minimum overlap-value
int newRStarTree::chooseSplitIndex(Node *currentNode,int &selectedAxis) {
    // Sort the entries by the lower and upper value of their rectangles based on the selected axis
    vector<Entry*> sortedEntries = currentNode->getEntries();
    sort(sortedEntries.begin(), sortedEntries.end(), [&selectedAxis](Entry *a, Entry *b) {
        return a->boundingBox->compareBoundingBox(*(b->boundingBox), selectedAxis);
    });

    // Choose the distribution with the mininum overlap-value
    // Resolve ties by choosing the distribution with minimum area-value
    double minOverlap = numeric_limits<double>::infinity();
    double minArea = numeric_limits<double>::infinity();
    int minOverlapIndex = 0;

    for (int k = 0; k < this->maxEntries - 2 * this->minEntries + 2; k++) {
        BoundingBox firstGroupBB(this->dimensions);
        BoundingBox secondGroupBB(this->dimensions);

        // Create the two groups of the MBBs
        for (int j = 0; j < this->minEntries - 1 + k; j++)
                firstGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
        for (int j = this->minEntries - 1 + k; j < this->maxEntries + 1; j++)
            secondGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
        
        double overlapValue = firstGroupBB.calculateOverlap(secondGroupBB);
        double areaValue = firstGroupBB.getArea() + secondGroupBB.getArea();
        if (overlapValue < minOverlap) {
            minOverlap = overlapValue;
            minOverlapIndex = k;
        } else if (overlapValue == minOverlap && areaValue < minArea) {
            minArea = areaValue;
            minOverlapIndex = k;
        }
    }

    return minOverlapIndex;
}

pair<Node*, Node*> newRStarTree::adjustTree(Node *currentNode, Node *newNode) {
    Entry *newEntry = new Entry();

    // While the current node is the not root move upwards
    while (currentNode->getParent() != nullptr) {
        // Adjust the MBB of the parent entry so that it tightly encloses all entry rectangles in the current node
        Node *parentNode = currentNode->getParent();
        parentNode->setLevel(currentNode->getLevel() - 1);
        Entry *currentNodeEntry = parentNode->findEntry(currentNode);

        currentNodeEntry->boundingBox = new BoundingBox(this->dimensions);
        for (auto entry : currentNode->getEntries()) {
            currentNodeEntry->boundingBox->includeBox(*entry->boundingBox);
        }

        if (newNode != nullptr) {
            newEntry->childNode = newNode;
            newNode->setParent(parentNode);
            newEntry->boundingBox = new BoundingBox(this->dimensions);

            for (auto entry : newNode->getEntries()) {
                newEntry->boundingBox->includeBox(*entry->boundingBox);
            }
            parentNode->insertEntry(newEntry);
            
            if (parentNode->entriesSize() > this->maxEntries) {
                Node *secondParentNode = new Node(false);
                parentNode->setLevel(parentNode->getLevel());
                splitNode(parentNode, secondParentNode);
                newNode = secondParentNode;
            } else {
                newNode = nullptr;
            }
        }

        currentNode = parentNode;
    }

    return make_pair(currentNode, newNode);
}

// Display the R* Tree using DFS traversal
void newRStarTree::display() {
    if (this->root == nullptr) {
        return;
    }

    stack<Node*> nodeStack;
    int p = 8;
    nodeStack.push(this->root);
    while (!nodeStack.empty()) {
        Node *current = nodeStack.top();
        nodeStack.pop();
        if (current->isLeafNode() && current->getEntries().size() > 0) {
            cout << "[";
            for (auto entry : current->getEntries()) {
                cout << "(" << findObjectById(*(entry->id), p) << ") ";
            }
            cout << "]" << endl;
        }
        for (auto entry : current->getEntries()) {
            if (entry->childNode == nullptr)
                continue;
            nodeStack.push(entry->childNode);
        }
    }
}