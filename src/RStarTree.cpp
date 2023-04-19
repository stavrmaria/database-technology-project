#include "RStarTree.h"

// Basic constructor for the R* Tree
RStarTree::RStarTree(int maxEntries, int dimensions) {
    this->dimensions = dimensions;
    this->root = new Node();
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
    return currentNode;
}

// Split a node to balance the nodes of the tree
void  RStarTree::splitNode(Node *currentNode, Node *newNode) {

}

// Insert a point into the R* Tree
void RStarTree::insert(Point &point) {

}