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

        Node *chooseSubtree(Entry*, Node*);
        void overFlowTreatment(Node*, Node*);
        pair<Node*, Node*> adjustTree(Node*, Node*);
        bool isFirstCallOfLevel(int);
        void reInsert(Node*, Node*);
        void splitNode(Node*, Node*);
        int chooseSplitAxis(Node*);
        int chooseSplitIndex(Node*,int&);
        void insert(Entry*, Node*);
        void destroyNode(Node*);
    public:
        newRStarTree(int, int, int);
        ~newRStarTree();
        Node *getRoot();
        unsigned long getNodesCount() const;
        void insertData(Point&, unsigned int&, unsigned int&);
        void display();
};

#endif