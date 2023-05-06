#include "RStarTree.h"

// Basic constructor for the R* Tree
RStarTree::RStarTree(int maxEntries, int dimensions) {
    this->dimensions = dimensions;
    this->root = new Node(true);
    this->maxEntries = maxEntries;
    this->minEntries = (int)(maxEntries / 2);
}

// Recursively delete the R* Tree
RStarTree::~RStarTree() {
    delete root;
}

// Return the root of the tree
Node *RStarTree::getRoot() {
    return this->root;
}

// Insert a point into the R* Tree
void RStarTree::insert(Point &point) {
    Node *currentNode = this->root;
    Node *leafNode = chooseLeaf(currentNode, point);
    Node *newNode = nullptr;
    Entry *newEntry = new Entry();

    newEntry->childNode = nullptr;
    newEntry->boundingBox = new BoundingBox(this->dimensions, point.getCoordinates(), point.getCoordinates());
    newEntry->point = point;
    leafNode->insertEntry(newEntry);

    // There is not available space to place the point
    if (leafNode->entriesSize() > this->maxEntries) {
        newNode = new Node(true);
        splitNode(leafNode, newNode);
    }

    // Adjust the structure of the tree based on the insertion
    pair<Node*, Node*> adjustedNodes = adjustTree(leafNode, newNode);
    leafNode = adjustedNodes.first;
    newNode = adjustedNodes.second;

    // The node split propagation caused the root to split, create a new root
    if (leafNode->getParent() == nullptr && newNode != nullptr) {
        Node *parentNode = new Node(false);
        
        Entry *firstEntry = new Entry();
        firstEntry->childNode = leafNode;
        leafNode->setParent(parentNode);
        firstEntry->boundingBox = new BoundingBox(this->dimensions);
        for (auto entry : leafNode->getEntries())
            firstEntry->boundingBox->includeBox(*entry->boundingBox);

        Entry *secondEntry = new Entry();
        secondEntry->childNode = newNode;
        newNode->setParent(parentNode);
        secondEntry->boundingBox = new BoundingBox(this->dimensions);
        for (auto entry : newNode->getEntries())
            secondEntry->boundingBox->includeBox(*entry->boundingBox);

        parentNode->insertEntry(firstEntry);
        parentNode->insertEntry(secondEntry);
        currentNode = parentNode;
    }

    this->root = currentNode;
}

// Find the appropriate leaf for a given point based on the MBBs
Node *RStarTree::chooseLeaf(Node *node,Point &point) {
    Node *currentNode = node;

    // Traverse the tree by selecting the entry whose rectangle needs
    // least enlargment to include the new point
    while (!currentNode->isLeafNode()) {
        Entry *entryToChoose = nullptr;
        double minEnlargment = numeric_limits<double>::infinity();
        vector<Entry*> entries = currentNode->getEntries();

        for (auto entry : entries) {
            BoundingBox updatedMBB = *(entry->boundingBox);
            BoundingBox pointBox(this->dimensions, point.getCoordinates(), point.getCoordinates());
            updatedMBB.includeBox(pointBox);
            double enlargement = updatedMBB.getArea() - entry->boundingBox->getArea();

            if (enlargement < minEnlargment) {
                entryToChoose = entry;
                minEnlargment = enlargement;
            }
        }

        currentNode = entryToChoose->childNode;
    }

    return currentNode;
}

// Split a node to balance the nodes of the tree
void RStarTree::splitNode(Node *currentNode, Node *newNode) {
    int firstSeedIndex = 0;
    int secondSeedIndex = 0;
    int counter = 0;
    vector<Entry*> entries = currentNode->getEntries();

    // Pick first entry for each group
    pickSeeds(currentNode, firstSeedIndex, secondSeedIndex);
    Entry firstSeed = *(entries.at(firstSeedIndex));
    Entry secondSeed = *(entries.at(secondSeedIndex));

    currentNode->clearEntries();
    newNode->clearEntries();

    BoundingBox firstBox = *(firstSeed.boundingBox);
    BoundingBox secondBox = *(secondSeed.boundingBox);
    for (int i = 0; i < entries.size(); i++) {
        Entry *entry = entries.at(i);
        double firstCost = firstBox.getArea();
        double secondCost = secondBox.getArea();     

        if (i == firstSeedIndex) {
            currentNode->insertEntry(entry);
            firstBox.includeBox(*entry->boundingBox);
            continue;
        }else if (i == secondSeedIndex) {
            newNode->insertEntry(entry);
            secondBox.includeBox(*entry->boundingBox);
            continue;
        }

        firstCost -= -firstBox.getUpdatedArea(*entry->boundingBox);
        secondCost -= -secondBox.getUpdatedArea(*entry->boundingBox);

        if ((firstCost < secondCost) || (currentNode->entriesSize() < newNode->entriesSize())) {
            currentNode->insertEntry(entry);
            firstBox.includeBox(*entry->boundingBox);
        } else {
            newNode->insertEntry(entry);
            secondBox.includeBox(*entry->boundingBox);
        }
    }
}

void RStarTree::pickNext(int &selected, int &firstSeedIndex, int &secondSeedIndex, vector<Entry*> entries) {
    double maxD = 0;
    BoundingBox firstBox = *(entries.at(firstSeedIndex)->boundingBox);
    BoundingBox secondBox = *(entries.at(secondSeedIndex)->boundingBox);

    for (int i = 0; i < entries.size(); i++) {
        double d1 = firstBox.getArea();
        firstBox.includeBox(*entries[i]->boundingBox);
        double d2 = secondBox.getArea();
        secondBox.includeBox(*entries[i]->boundingBox);
        double d = abs(d1 - d2);

        if (d > maxD) {
            maxD = d;
            selected = i;
        }
    }
}

pair<Node*, Node*> RStarTree::adjustTree(Node *currentNode, Node *newNode) {
    Entry *newEntry = new Entry();

    // While the current node is the not root move upwards
    while (currentNode->getParent() != nullptr) {
        // Adjust the MBB of the parent entry so that it tightly encloses all entry rectangles in the current node
        Node *parentNode = currentNode->getParent();
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

void RStarTree::pickSeeds(Node *currentNode, int &firstSeedIndex, int &secondSeedIndex) {
    vector<Entry*> entries = currentNode->getEntries();
    double maxD = 0;

    // Calculate inefficiency of grouping entries together
    for (int firstSeed = 0; firstSeed < entries.size(); firstSeed++) {
        for (int secondSeed = 0; secondSeed < firstSeed; secondSeed++) {
            BoundingBox tempBox(this->dimensions);
            tempBox.includeBox(*entries.at(firstSeed)->boundingBox);
            tempBox.includeBox(*entries.at(secondSeed)->boundingBox);

            double d = tempBox.getArea() - entries.at(firstSeed)->boundingBox->getArea() - entries.at(secondSeed)->boundingBox->getArea();
            if (d > maxD) {
                maxD = d;
                firstSeedIndex = firstSeed;
                secondSeedIndex = secondSeed;
            }
        }
    }
}

// Traverse the tree and print it's leaves
void  RStarTree::traverse(Node *currentNode) {
    if (currentNode == nullptr) {
        return;
    }

    if (currentNode->isLeafNode()) {
        // Process leaf node
        cout << "[";
        for (auto entry : currentNode->getEntries()) {
            cout << entry->point.getID() << ",";
        }
        cout << "]" << endl;
    } else {
        // Process non-leaf node
        for (auto entry : currentNode->getEntries()) {
            // Traverse child node recursively
            traverse(entry->childNode);
        }
    }
}

void  RStarTree::traverse() {
    traverse(this->root);
}