#include "RStarTree.h"
#include "Node.h"
#include <queue>

// Implementation of range query
vector<ID> newRStarTree::rangeQuery(BoundingBox &boundingBox) {
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

struct comparePairs {
    bool operator()(const pair<double, ID> &a, const pair<double, ID> &b) {
        return a.first > b.first;
    }
};

// Implementation of k-nearest neighbors query
vector<ID> newRStarTree::kNearestNeighbors(Point& queryPoint, int k) {
    vector<ID> kNeighbors = {};

    if (this->root == nullptr) {
        return kNeighbors;
    }

    // return kNeighbors;
    // Priority queue to store the nearest neighbors
    priority_queue<pair<double, ID>, vector<pair<double, ID>>, comparePairs> nearestNeighbors;
    stack<Node*> node;
    node.push(this->root);

    while (!node.empty()) {
        Node* current = node.top();
        node.pop();

        if (current->isLeafNode()) {
            for (auto entry : current->getEntries()) {

                //SOS HOW TO TAKE THE POINT FROM AN ENTRY
                Point entryPoint(entry->boundingBox->getMinCoordinates());
                double distance = queryPoint.getDistance(entryPoint);
                if (nearestNeighbors.size() < k || distance < nearestNeighbors.top().first) {
                    // Add the distance and ID pair to the priority queue
                    nearestNeighbors.push(make_pair(distance, *(entry->id)));

                    // If the number of neighbors exceeds k, remove the farthest neighbor
                    if (nearestNeighbors.size() > k) {
                        nearestNeighbors.pop();
                    }
                }
            }
        }

        // Push child nodes onto the stack
        for (auto entry : current->getEntries()) {
            if (entry->childNode == nullptr) {
                continue;
            }
            node.push(entry->childNode);
        }
    }

    // Extract the IDs of the nearest neighbors from the priority queue
    while (!nearestNeighbors.empty()) {
        kNeighbors.push_back(nearestNeighbors.top().second);
        nearestNeighbors.pop();
    }

    // Reverse the results to obtain the nearest neighbors in ascending order of distance
    reverse(kNeighbors.begin(), kNeighbors.end());

    return kNeighbors;
}