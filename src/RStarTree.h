#ifndef newRStarTree_h
#define newRStarTree_h

#include "Point.h"
#include "Node.h"
#include "constants.h"

#include <stack>
#include <sstream>
#include <iostream>
#include <vector>
#include <limits>
#include <queue>

using namespace std;

void createChildEntry(Node*, Node*);

class newRStarTree {
private:
    Node *root;
    int minEntries;
    int maxEntries;
    int dimensions;
    int maxObjectSize;
    unsigned long nodesCount;
    vector<int> levelCallMap;

    Node *chooseSubtree(Entry*, Node*, int);
    Node *overFlowTreatment(Node*);
    void adjustTree(Node*);
    bool isFirstCallOfLevel(int);
    void reInsert(Node*);
    void splitNode(Node*, Node*);
    int chooseSplitAxis(Node*) const;
    int chooseSplitIndex(Node*,int&) const;
    void insert(Entry*, Node*, int);
    void destroyNode(Node*);
    void saveIndex(fstream& indexFile, Node *currentNode);
    void saveData(fstream& dataFile, Node *currentNode);
public:
    newRStarTree(int, int, int);
    ~newRStarTree();
    void setRoot(Node*);
    Node *getRoot();
    void insertData(Point&, unsigned int&, unsigned int&);
    void display();
    vector<ID> rangeQuery(BoundingBox &);
    vector<ID> kNearestNeighbors(Point& queryPoint, int k);
    vector<ID> skylineQuery();
    void deletePoint(Point&);
    Entry* findEntryToDelete(Point& , Node*);
    void removeEntry(Entry* , Node*);
    void condenseTree(Node*);
    void adjustNonLeafNode(Node* , Node*);
    int saveIndex(const string &indexFileName);
    int saveData(const string &dataFileName);

    void deletePoint(Point&);
    pair<Entry*, Node*> findEntryToDelete(Point& , Node*);
    void condenseTree(Node*);
    void adjustNonLeafNode(Node* , Node*);
};

#endif