#ifndef Node_h
#define Node_h

#include <iostream>
#include <vector>
#include <algorithm>
#include "Point.h"
#include "BoundingBox.h"

using namespace std;

// Class that represents the node of a tree
struct Entry;
struct Block;
class Node {
    private:
        bool isLeaf;
        Node *parent;
        vector<Entry*> entries;
    public:
        Node();
        Node(bool);
        Node(vector<Entry*>);
        Node *getParent();
        void setParent(Node*);
        bool isLeafNode() const;
        Entry *findEntry(int&);
        Entry *findEntry(Node*);
        vector<Entry*> getEntries();
        int entriesSize();
        void insertEntry(Entry*);
        void clearEntries();
};

// Struct that represents each entry of a given node
struct Entry {
    BoundingBox *boundingBox = nullptr;
    Node *childNode = nullptr;
    Point point;
};

#endif