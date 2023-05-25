#include "RStarTree.h"
#include "Node.h"

// Implementation of range query
vector<ID> RStarTree::rangeQuery(BoundingBox &boundingBox) {
    vector<ID> results = {};

    if (this->root == nullptr) {
        return results;
    }

    stack<Node*> nodeStack;
    nodeStack.push(this->root);
    while (!nodeStack.empty()) {
        Node *current = nodeStack.top();
        nodeStack.pop();

        if (current->isLeafNode()) {
            for (auto entry : current->getEntries()) {
                if (!entry->boundingBox->intersects(boundingBox))
                    continue;
                results.push_back(*(entry->id));
            }
        }

        // Push child nodes onto the stack if they insersect with
        // the bounding box of the query
        for (auto entry : current->getEntries()) {
            if (!entry->boundingBox->intersects(boundingBox) || entry->childNode == nullptr)
                continue;
            nodeStack.push(entry->childNode);
        }
    }

    return results;
}

// Display the R* Tree using DFS traversal
void RStarTree::display() {
    if (this->root == nullptr) {
        return;
    }

    stack<Node*> nodeStack;
    nodeStack.push(this->root);
    while (!nodeStack.empty()) {
        Node *current = nodeStack.top();
        nodeStack.pop();
        if (current->isLeafNode()) {
            cout << "[";
            for (auto entry : current->getEntries()) {
                cout << findObjectById(*(entry->id), maxObjectSize) << endl;
            }
            cout << "]" << endl;
        }
        for (auto entry : current->getEntries()) {
            if (entry->childNode == nullptr)
                continue;
            nodeStack.push(entry->childNode);
        }
    }
}