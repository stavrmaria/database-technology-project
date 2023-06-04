#ifndef RStarTree_h
#define RStarTree_h

#include "Point.h"
#include "Node.h"
#include "constants.h"
#include <stack>
#include <sstream>


#include <iostream>
#include <vector>
#include <limits>
#include <fstream>

using namespace std;

class RStarTree {
    private:
        Node *root;
        int minEntries;
        int maxEntries;
        int dimensions;
        int maxObjectSize;
        unsigned long nodesCount;
        Node *chooseLeaf(Node*,Point&);
        void splitNode(Node*, Node*);
        pair<Node*, Node*> adjustTree(Node*, Node*);
        void pickSeeds(Node*, int&, int&);
        void pickNext(int&, int&, int&, vector<Entry*>);
        void traverse(Node*);
        void condenseTree(Node*);
        void insertEntry(Entry*);
        void deleteChild(Node*);
        Node *chooseLeafEntry(Node*,Entry*);

        void saveIndex(fstream&, Node*);
        void saveData(fstream&, Node*);
    public:
        RStarTree(int, int, int);
        ~RStarTree();
        Node *getRoot();
        unsigned long getNodesCount() const;
        void insert(Point&, unsigned int&, unsigned int&);
        void insert(Point&);
        void traverse();
        void deletePoint(Point&);

        int saveIndex(const string&);
        int saveData(const string&);
        vector<ID> rangeQuery(BoundingBox&);
        vector<ID> kNearestNeighbors(Point&, int);
        void display();
};

#endif