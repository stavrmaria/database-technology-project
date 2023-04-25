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
        vector<Entry*> entries;
    public:
        Node();
        Node(bool);
        Node(vector<Entry*>);
        bool isLeafNode() const;
        Entry *findEntry(int&);
        vector<Entry*> getEntries();
        int entriesSize();
        void insertEntry(Entry*);
};

// Struct that represents each entry of a given node
struct Entry {
    BoundingBox *boundingBox = nullptr;
    Node *childNode = nullptr;
    string pointID = "";
};

#endif