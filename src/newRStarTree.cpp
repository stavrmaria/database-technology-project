#include "newRStarTree.h"

// Basic constructor for the R* Tree
newRStarTree::newRStarTree(int maxEntries, int dimensions, int maxObjectSize) {
    this->dimensions = dimensions;
    this->root = new Node(true);
    this->maxEntries = maxEntries;
    this->minEntries = (int)(maxEntries / 2);
    this->nodesCount = 1;
    this->maxObjectSize = maxObjectSize;
}

// Recursively delete the R* Tree
newRStarTree::~newRStarTree() {
    delete root;
}

// Return the root of the tree
Node *newRStarTree::getRoot() {
    return this->root;
}

unsigned long newRStarTree::getNodesCount() const {
    return this->nodesCount;
}

/***** Insertion Functions *****/
// Insert a point into the R* Tree
void newRStarTree::insert(Point &point, unsigned int &blockID, unsigned int &slot) {
    Node *currentNode = this->root;
    Node *subtreeNode = chooseSubtree(currentNode, point);
    Node *newNode = nullptr;
    Entry *newEntry = new Entry();

    newEntry->childNode = nullptr;
    newEntry->boundingBox = new BoundingBox(this->dimensions, point.getCoordinates(), point.getCoordinates());
    newEntry->id = new ID;
    newEntry->id->blockID = blockID;
    newEntry->id->slot = slot;
    subtreeNode->insertEntry(newEntry);

    // There is not available space to place the point
    if (subtreeNode->entriesSize() > this->maxEntries) {
        newNode = new Node(true);
        overFlowTreatment(subtreeNode);
    }

    this->root = currentNode;
    this->nodesCount++;
}

// Find the appropriate entry for a given point based on the MBBs
Node *newRStarTree::chooseSubtree(Node *node,Point &point) {
    Node *currentNode = node;

    while (!currentNode->isLeafNode()) {
        Entry *entryToChoose = nullptr;
        double minEnlargment = numeric_limits<double>::infinity();
        vector<Entry*> entries = currentNode->getEntries();

        // Traverse the entries of the current node and select the entry whose rectangle needs
        // least (area/overlap) enlargment to include the new point
        for (auto entry : entries) {
            BoundingBox updatedMBB = *(entry->boundingBox);
            BoundingBox pointBox(this->dimensions, point.getCoordinates(), point.getCoordinates());
            updatedMBB.includeBox(pointBox);
            double enlargement;

            if (currentNode->entriesSize() > 0 && currentNode->getEntries().at(0)->childNode->isLeafNode())
                enlargement = updatedMBB.calculateOverlap(entry->boundingBox->getArea());
            else
                enlargement = updatedMBB.getArea() - entry->boundingBox->getArea();

            if (enlargement < minEnlargment) {
                entryToChoose = entry;
                minEnlargment = enlargement;
            }
        }
        currentNode = entryToChoose->childNode;
    }
    
    return currentNode;
}

void newRStarTree::overFlowTreatment(Node* currentNode) {
    if (currentNode->getParent() != nullptr && isFirstCallOfLevel(currentNode->getLevel()))
        reInsert(currentNode);
    split(currentNode);
}

bool newRStarTree::isFirstCallOfLevel(int level) {
    // Check if the level exists in the map
    if (this->levelCallMap.find(level) == this->levelCallMap.end()) {
        // Level not found, this is the first call
        this->levelCallMap[level] = true;
        return true;
    }

    // Level found, not the first call
    return false;
}

void newRStarTree::reInsert(Node *currentNode) {

}

void newRStarTree::split(Node *currentNode) {
    int selectedAxis = chooseSplitAxis(currentNode);
    int selectedDistributionIndex = chooseSplitIndex(currentNode, selectedAxis);
    BoundingBox firstGroupBB(this->dimensions);
    BoundingBox secondGroupBB(this->dimensions);
    vector<Entry*> sortedEntries = currentNode->getEntries();

    sort(sortedEntries.begin(), sortedEntries.end(), [&selectedAxis](Entry *a, Entry *b) {
        double minALower = a->boundingBox->getMinCoordinates().at(selectedAxis);
        double minBLower = b->boundingBox->getMinCoordinates().at(selectedAxis);
        double minAUpper = a->boundingBox->getMaxCoordinates().at(selectedAxis);
        double minBUpper = b->boundingBox->getMaxCoordinates().at(selectedAxis);
        return (minALower < minBLower) || (minALower == minBLower && minAUpper < minBUpper);
    });

    // Create the two groups of the MBBs
    for (int j = 0; j <= selectedDistributionIndex; j++)
        firstGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
    for (int j = selectedDistributionIndex + 1; j < sortedEntries.size(); j++)
        secondGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
}

int newRStarTree::chooseSplitAxis(Node *currentNode) {
    int minSIndex = 0;
    double minS = numeric_limits<double>::infinity();

    for (int i = 0; i < this->dimensions; i++) {
        // Sort the entries by the lower and upper value of their rectangles for the current axis
        vector<Entry*> sortedEntries = currentNode->getEntries();
        sort(sortedEntries.begin(), sortedEntries.end(), [&i](Entry *a, Entry *b) {
            double minALower = a->boundingBox->getMinCoordinates().at(i);
            double minBLower = b->boundingBox->getMinCoordinates().at(i);
            double minAUpper = a->boundingBox->getMaxCoordinates().at(i);
            double minBUpper = b->boundingBox->getMaxCoordinates().at(i);
            return (minALower < minBLower) || (minALower == minBLower && minAUpper < minBUpper);
        });

        for (int k = 0; k < sortedEntries.size(); k++) {
            BoundingBox firstGroupBB(this->dimensions);
            BoundingBox secondGroupBB(this->dimensions);

            // Create the two groups of the MBBs
            for (int j = 0; j < this->maxEntries - 1 + k; j++)
                firstGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
            for (int j = this->maxEntries - 2 + k; j < sortedEntries.size(); j++)
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

// Along the chosen split axIs, choose the distribution with the minimum overlap-value
int newRStarTree::chooseSplitIndex(Node *currentNode,int &selectedAxis) {
    // Sort the entries by the lower and upper value of their rectangles for the current axis
    vector<Entry*> sortedEntries = currentNode->getEntries();
    sort(sortedEntries.begin(), sortedEntries.end(), [&selectedAxis](Entry *a, Entry *b) {
        double minALower = a->boundingBox->getMinCoordinates().at(selectedAxis);
        double minBLower = b->boundingBox->getMinCoordinates().at(selectedAxis);
        double minAUpper = a->boundingBox->getMaxCoordinates().at(selectedAxis);
        double minBUpper = b->boundingBox->getMaxCoordinates().at(selectedAxis);
        return (minALower < minBLower) || (minALower == minBLower && minAUpper < minBUpper);
    });

    // Choose the distribution with the mininum overlap-value
    // Resolve ties by choosing the dlstrlbutlon with minimum area-value
    double minOverlap = numeric_limits<double>::infinity();
    double minArea = numeric_limits<double>::infinity();
    int minOverlapIndex = 0;
    for (int k = 0; k < sortedEntries.size(); k++) {
        BoundingBox firstGroupBB(this->dimensions);
        BoundingBox secondGroupBB(this->dimensions);

        // Create the two groups of the MBBs
        for (int j = 0; j < this->maxEntries - 1 + k; j++)
            firstGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
        for (int j = this->maxEntries - 1 + k; j < sortedEntries.size(); j++)
            secondGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
        
        double overlapValue = firstGroupBB.calculateOverlap(secondGroupBB);
        double areaValue = firstGroupBB.getArea() + secondGroupBB.getArea();
        if (overlapValue < minOverlap) {
            minOverlap = overlapValue;
            minArea = areaValue;
            minOverlapIndex = k;
        } else if (overlapValue == minOverlap && minArea < areaValue) {
            minArea = areaValue;
            minOverlapIndex = k;
        }
    }

    return minOverlapIndex;
}
