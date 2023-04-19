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
class Node {
    private:
        bool isLeaf;
        vector<Entry*> entries;
    public:
        Node();
        Node(vector<Entry*>);
        bool isLeafNode() const;
        Entry *findEntry(int&);
        int nodeSize();
        void insertEntry(Entry*);
};

struct Entry {
    BoundingBox *boundingBox;
    Node *childNode;
    Point *pointID;
};

#endif