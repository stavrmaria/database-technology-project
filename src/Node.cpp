#include "Node.h"

// Empty node constructor with no entries
Node::Node() {
    this->entries.clear();
    this->parent = nullptr;
    this->isLeaf = false;
    this->level = 0;
}

// Node constructor with no entries
Node::Node(int dimensions, bool isLeaf) {
    this->entries.clear();
    this->parent = nullptr;
    this->isLeaf = isLeaf;
    this->level = 0;
    this->dimensions = dimensions;
}

// Node constructor based on a vector of entries (leaf/non-leaf)
Node::Node(int dimensions, vector<Entry*> entries) {
    this->entries = entries;
    this->parent = nullptr;
    this->level = 0;
    if (entries.at(0)->childNode != nullptr)
        this->isLeaf = false;
    else
        this->isLeaf = true;
    this->dimensions = dimensions;
}

// Node destructor
Node::~Node() {
    entries.clear();
    delete parent;
}

// Get the parent of the node
Node *Node::getParent() {
    return this->parent;
}

// Check whether the node is leaf
bool Node::isLeafNode() const {
    return this->isLeaf;
}

// Find an entry in the node based on a node
Entry* Node::findEntry(Node* target) {
    if (this == nullptr)
        return nullptr;

    for (auto entry : entries) {
        if (*(entry->childNode) == *target)
            return entry;
    }

    return nullptr;
}

// Return the entries of the node
vector<Entry*> Node::getEntries() {
    return this->entries;
}

// Return the level of the node
int Node::getLevel() const {
    return this->level;
}

int Node::getDimensions() const {
    return this->dimensions;
}

// Set the level of the node
void Node::setLevel(int level) {
    this->level = level;
    if (level == 0)
        this->isLeaf = true;
    else
        this->isLeaf = false;
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
void Node::insertEntry(Entry *newEntry) {
    this->entries.push_back(newEntry);
}

// Clear all the entries from the node
void Node::clearEntries() {
    this->entries.clear();
}

void Node::clearBoudingBox(){
    for(auto entry: entries){
        delete entry->boundingBox;
        entry->boundingBox = nullptr;
    }
}

void Node:: deleteEntry(Entry* target) {
    if (this == nullptr)
        return;

    for (auto it = entries.begin(); it != entries.end(); ++it) {
        if (*it == target) {
            entries.erase(it);
            break;
        }
    }
}

void Node::removeChild(Node *child) {
    for (int i = 0; i < entries.size(); i++) {
        if (entries[i]->childNode == child) {
            entries.erase(entries.begin() + i);
            break;
        }
    }
}

// Find the entry in the current node whose rectangle needs least overlap enlargement
Entry* Node::minOverlapEntry(const Entry* newEntry, double& enlargement) const {
    Entry* entryToChoose = entries.at(0);
    double minOverlapEnlargement = entryToChoose->boundingBox->calculateOverlap(*(newEntry->boundingBox));

    for (auto entry : entries) {
        double overlapEnlargement = entry->boundingBox->calculateOverlap(*(newEntry->boundingBox));
        if (overlapEnlargement < minOverlapEnlargement) {
            minOverlapEnlargement = overlapEnlargement;
            entryToChoose = entry;
        }
    }

    enlargement = minOverlapEnlargement;
    return entryToChoose;
}

// Find the entry in the current node whose rectangle needs least overlap enlargement
Entry* Node::minEnlargedAreaEntry(const Entry *newEntry, double& enlargment) const {
    Entry* entryToChoose = entries.at(0);
    double minEnlargement = numeric_limits<double>::infinity();

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

    enlargment = minEnlargement;
    return entryToChoose;
}

bool Node::operator==(const Node &other) const {
    return other.entries == this->entries && other.parent == this->parent;
}

BoundingBox* Node::adjustBoundingBoxes() {
    if (entries.empty()) {
        return nullptr;
    }

    BoundingBox* adjustedBox = nullptr;

    for (const auto& entry : entries) {
        BoundingBox* childBox = entry->boundingBox;
        if (adjustedBox == nullptr) {
            adjustedBox = new BoundingBox(*childBox);
        } else {
            adjustedBox->getUpdatedArea(*childBox);
        }
    }

    return adjustedBox;
}
    
void Node::serializeNode(fstream& indexFile) {
    indexFile.write(reinterpret_cast<const char*>(&level), sizeof(int));
    indexFile.write(reinterpret_cast<const char*>(&dimensions), sizeof(int));
    indexFile.write(reinterpret_cast<const char*>(&isLeaf), sizeof(bool));

    int n = entries.size();
    indexFile.write(reinterpret_cast<const char*>(&n), sizeof(int));

    for (const auto& entry : entries) {
        // Serialize the entry
        if (entry->id != nullptr) {
            indexFile.write(reinterpret_cast<const char*>(&entry->id->blockID), sizeof(unsigned long));
            indexFile.write(reinterpret_cast<const char*>(&entry->id->slot), sizeof(unsigned long));
        } else {
            unsigned long k = 0;
            indexFile.write(reinterpret_cast<const char*>(&k), sizeof(unsigned long));
            indexFile.write(reinterpret_cast<const char*>(&k), sizeof(unsigned long));
        }

        if (entry->boundingBox != nullptr) {
            entry->boundingBox->serialize(indexFile);
        }
    }
}

void Node::deserializeNode(ifstream& indexFile) {
    indexFile.read(reinterpret_cast<char*>(&level), sizeof(int));
    indexFile.read(reinterpret_cast<char*>(&dimensions), sizeof(int));
    indexFile.read(reinterpret_cast<char*>(&isLeaf), sizeof(bool));

    if (level == 0)
        isLeaf = true;

    int n;
    indexFile.read(reinterpret_cast<char*>(&n), sizeof(int));
    entries.clear();

    for (int i = 0; i < n; i++) {
        auto entry = new Entry();
        entry->id = new ID();
        entry->boundingBox = new BoundingBox(dimensions);

        indexFile.read(reinterpret_cast<char*>(&entry->id->blockID), sizeof(unsigned long));
        indexFile.read(reinterpret_cast<char*>(&entry->id->slot), sizeof(unsigned long));
        if (entry->id->blockID == 0 && entry->id->slot == 0) {
            delete entry->id;
            entry->id = nullptr;
        }

        entry->boundingBox->deserialize(indexFile);  // Deserialize the bounding box

        // Initialize child node to nullptr
        entry->childNode = nullptr;

        entries.push_back(entry);
    }
}
