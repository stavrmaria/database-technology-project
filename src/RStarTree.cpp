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
            updatedMBB.update(point);

            double enlargement = updatedMBB.getArea(); - entry->boundingBox->getArea();

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
void  RStarTree::splitNode(Node *currentNode, Node *newNode) {

}

Node *RStarTree::getRoot() {
    return this->root;
}

// Insert a point into the R* Tree
void RStarTree::insert(Point &point) {
    Node *currentNode = this->root;
    Node *leafNode = chooseLeaf(currentNode, point);
    Node *newNode = nullptr;

    // There is root to place the point
    if (leafNode->entriesSize() < this->maxEntries) {
        Entry *newEntry = new Entry();
        newEntry->childNode = nullptr;
        newEntry->pointID = point.getID();
        leafNode->insertEntry(newEntry);
    } else {
        newNode = new Node(true);
        splitNode(leafNode, newNode);
    }

    adjustTree(leafNode, newNode);
}

void RStarTree::splitNode(Node *currentNode, Node *newNode) {

}

void RStarTree::adjustTree(Node *currentNode, Node *newNode) {
    
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
            cout << entry->pointID << ",";
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