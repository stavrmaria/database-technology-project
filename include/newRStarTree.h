#ifndef newRStarTree_h
#define newRStarTree_h

#include "Point.h"
#include "Node.h"
#include "constants.h"

#include <stack>
#include <sstream>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <limits>
#include <fstream>

using namespace std;

class newRStarTree {
    private:
        Node *root;
        int minEntries;
        int maxEntries;
        int dimensions;
        int maxObjectSize;
        unsigned long nodesCount;
        unordered_map<int, bool> levelCallMap;

        Node *chooseSubtree(Node*,Point&);
        void overFlowTreatment(Node*);
        bool isFirstCallOfLevel(int);
        void reInsert(Node*);
        void split(Node*);
        int chooseSplitAxis(Node*);
        int chooseSplitIndex(Node*,int&);
    public:
        newRStarTree(int, int, int);
        ~newRStarTree();
        Node *getRoot();
        unsigned long getNodesCount() const;
        void insert(Point&, unsigned int&, unsigned int&);
};

#endif