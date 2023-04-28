#include "Node.h"

// Empty node constructor with no entries
Node::Node() {
    this->entries.clear();
    this->parent = nullptr;
    this->isLeaf = false;
}

// Node constructor with no entries
Node::Node(bool isLeaf) {
    this->entries.clear();
    this->parent = nullptr;
    this->isLeaf = isLeaf;
}

// Node constructor based on a vector of entries (leaf/non-leaf)
Node::Node(vector<Entry*> entries) {
    this->entries = entries;
    this->parent = nullptr;
    if (entries.at(0)->childNode != nullptr)
        this->isLeaf = false;
    else
        this->isLeaf = true;
}

// Get the parent of the node
Node *Node::getParent() {
    return this->parent;
}

// Check whether or not the node is leaf 
bool Node::isLeafNode() const {
    return this->isLeaf;
}

// Find an entry in the node based on index
Entry *Node::findEntry(int &index) {
    return this->entries.at(index);
}

// Find an entry in the node based on a node
Entry *Node::findEntry(Node *node) {
    if (this == nullptr)
        return nullptr;
    
    for (auto &entry : this->entries) {
        if (entry->childNode == node)
            return entry;
    }
}

// Return thr entries of the node
vector<Entry*> Node::getEntries() {
    return this->entries;
}

// Set the parent of the node
void Node::setParent(Node *parent) {
    this->parent = parent;
}

// Get the number of entries for this node
int Node::entriesSize() {
    return this->entries.size();
}

// Insert a new entry in the node
void Node::insertEntry(Entry* entry) {
    this->entries.push_back(entry);
}

// Clear all the entries from the node
void Node::clearEntries() {
    this->entries.clear();
}