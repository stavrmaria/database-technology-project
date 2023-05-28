#include "Node.h"

// Empty node constructor with no entries
Node::Node() {
    this->entries.clear();
    this->parent = nullptr;
    this->isLeaf = false;
    this->level = 0;
}

// Node constructor with no entries
Node::Node(bool isLeaf) {
    this->entries.clear();
    this->parent = nullptr;
    this->isLeaf = isLeaf;
    this->level = 0;
}

// Node constructor based on a vector of entries (leaf/non-leaf)
Node::Node(vector<Entry*> entries) {
    this->entries = entries;
    this->parent = nullptr;
    this->level = 0;
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

// Return the entries of the node
vector<Entry*> Node::getEntries() {
    return this->entries;
}

// Return the level of the node
int Node::getLevel() const {
    return this->level;
}

// Set the level of the node
void Node::setLevel(int level) {
    this->level = level;
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

void Node::clearBoudingBox(){
    for(auto entry: entries){
        delete entry->boundingBox;
        entry->boundingBox=nullptr;
    }
}

void Node::adjustBoundingBoxes() {
    if (entries.empty()) {
        // No entries in the node, clear the bounding box
        clearBoudingBox();
    } else {
        // Update the bounding box based on the entries
        clearBoudingBox();
        for (auto entry : entries) {
            entry->boundingBox->includeBox(*(entry->boundingBox));
        }
    }

    if (parent != nullptr) {
        // Recursively adjust the parent's bounding box
        parent->adjustBoundingBoxes();
    }
}

void Node:: deleteEntry(Entry* entry) {
    Node* parentNode = entry->childNode->getParent();
    parentNode->deleteEntry(entry);

    // Adjust bounding boxes on the path to the root
    while (parentNode != nullptr) {
        parentNode->adjustBoundingBoxes();
        parentNode = parentNode->getParent();
    }
}

void Node::removeChild(Node *child) {
    for (int i = 0; i < entries.size(); i++) {
        if (entries[i]->childNode == child) {
            entries.erase(entries.begin() + i);
            return;
        }
    }
}

// Find the entry in the current node whose rectangle needs least overlap enlargement
Entry* Node::minOverlapEntry(const Entry* newEntry, double* enlargement) const {
    double minOverlapEnlargement = numeric_limits<double>::infinity();
    Entry* entryToChoose = entries.at(0);

    for (auto entry : entries) {
        BoundingBox originalEntryBB = *(entry->boundingBox);
        BoundingBox updatedEntryBB = *(entry->boundingBox);
        updatedEntryBB.includeBox(*(newEntry->boundingBox));
        double overlapEnlargement = originalEntryBB.calculateOverlap(updatedEntryBB);

        if (overlapEnlargement < minOverlapEnlargement) {
            minOverlapEnlargement = overlapEnlargement;
            entryToChoose = entry;
        }
    }

    *enlargement = minOverlapEnlargement;
    return entryToChoose;
}

// Find the entry in the current node whose rectangle needs least overlap enlargement
Entry* Node::minEnlargedAreaEntry(const Entry *newEntry, double *enlargment) const {
    double minEnlargement = numeric_limits<double>::infinity();
    Entry* entryToChoose = entries.at(0);
    for (auto entry : entries) {
        BoundingBox originalEntryBB = *(entry->boundingBox);
        BoundingBox updatedEntryBB = *(entry->boundingBox);
        updatedEntryBB.includeBox(*(newEntry->boundingBox));
        double areaEnlargement = updatedEntryBB.getArea() - originalEntryBB.getArea();
        if (areaEnlargement < minEnlargement) {
            minEnlargement = areaEnlargement;
            entryToChoose = entry;
        }
    }

    *enlargment = minEnlargement;
    return entryToChoose;
}