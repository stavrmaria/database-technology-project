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
struct ID;
class Node {
    private:
        bool isLeaf;
        int level;
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
        void setLevel(int);
        int getLevel() const;
        void insertEntry(Entry*);
        void clearEntries();
        void adjustBoundingBoxes();
        void clearBoudingBox();
        void deleteEntry(Entry*);
        void removeChild(Node *);
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