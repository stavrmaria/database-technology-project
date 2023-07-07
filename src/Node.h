#ifndef Node_h
#define Node_h

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "Point.h"
#include "BoundingBox.h"

using namespace std;

// Class that represents the node of a tree
struct Entry;
struct ID;
class Node {
    private:
        bool isLeaf;
        int level;
        int dimensions;
        Node *parent;
        vector<Entry*> entries;
    public:
        Node();
        Node(int, bool);
        Node(int, vector<Entry*>);
        ~Node();
        Node *getParent();
        void setParent(Node*);
        bool isLeafNode() const;
        int getDimensions() const;
        Entry *findEntry(Node*);
        vector<Entry*> getEntries();
        int entriesSize();
        void setLevel(int);
        int getLevel() const;
        void insertEntry(Entry*);
        void clearEntries();
        BoundingBox* adjustBoundingBoxes();
        void clearBoudingBox();
        void deleteEntry(Entry*);
        void removeChild(Node *);
        Entry* minOverlapEntry(const Entry*, double&) const;
        Entry *minEnlargedAreaEntry(const Entry*, double&) const;
        bool operator==(const Node& other) const;
        void serializeNode(fstream&);
        void deserializeNode(ifstream&);
};

// Struct that represents each entry of a given node
struct Entry {
    BoundingBox *boundingBox = nullptr;
    Node *childNode = nullptr;
    ID *id = nullptr;
};

struct ID {
    unsigned long blockID = 0;
    unsigned long slot = 0;
};

#endif