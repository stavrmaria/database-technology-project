#ifndef RStarTree_h
#define RStarTree_h

#include "Point.h"
#include "Node.h"

#include <iostream>
#include <vector>
#include <limits>

using namespace std;

class RStarTree {
    private:
        Node *root;
        int minEntries;
        int maxEntries;
        int dimensions;
        Node *chooseLeaf(Node*,Point&);
        void splitNode(Node*, Node*);
    public:
        RStarTree(int, int);
        ~RStarTree();
        void insert(Point&);
};

#endif