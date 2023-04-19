#include "Node.h"

// Empty node constructor with no entries
Node::Node() {
    this->entries.clear();
    this->isLeaf = false;
}

// Node constructor based on a vector of entries (leaf/non-leaf)
Node::Node(vector<Entry*> entries) {
    this->entries = entries;
    if (entries.at(0)->pointID == nullptr)
        this->isLeaf = false;
    else
        this->isLeaf = true;
}

// Check whether or not the node is leaf 
bool Node::isLeafNode() const {
    return this->isLeaf;
}

// Find an entry in the node
Entry *Node::findEntry(int &index) {
    return this->entries.at(index);
}

// Get the number of entries for this node
int Node::nodeSize() {
    return this->entries.size();
}

// Insert a new entry in the node
void Node::insertEntry(Entry* entry) {
    this->entries.push_back(entry);
}
