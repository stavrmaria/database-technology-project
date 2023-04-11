#ifndef Node_h
#define Node_h

#include <iostream>
#include <vector>
#include <algorithm>
#include "Point.h"
#include "BoundingBox.h"
#include "constants.h"

using namespace std;

class Node {
    private:
        Node *parent;
        BoundingBox boundingBox;
        bool isLeaf;
    public:
        Node();
        BoundingBox getBoundingBox();
        Node* getParent();
        void setParent(Node*);
        bool checkIsLeaf() const;
        void setIsLeaf(bool);
        void updateBoundingBox();
};

#endif