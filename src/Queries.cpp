#include "RStarTree.h"
#include "Node.h"
#include <queue>
#include <set>

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
        return a.first < b.first;
    }
};

// Implementation of k-nearest neighbors query
vector<ID> newRStarTree::kNearestNeighbors(Point& queryPoint, int k) {
    vector<ID> kNeighbors = {};
    if (this->root == nullptr) {
        return kNeighbors;
    }

    // Priority queue to store the nearest neighbors
    set<pair<double, ID>, comparePairs> nearestNeighbors;
    stack<Node*> node;
    node.push(this->root);

    while (!node.empty()) {
        Node* current = node.top();
        node.pop();
        if (current->isLeafNode()) {
            for (auto entry : current->getEntries()) {
                Point entryPoint(entry->boundingBox->getMinCoordinates());
                double distance = queryPoint.getDistance(entryPoint);

                if (nearestNeighbors.size() < k || distance < nearestNeighbors.rbegin()->first) {
                    // Add the distance and ID pair to the priority queue
                    nearestNeighbors.insert(make_pair(distance, *(entry->id)));
                    // If the number of neighbors exceeds k, remove the farthest neighbor
                    if (nearestNeighbors.size() > k)
                        nearestNeighbors.erase(--nearestNeighbors.end());
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
    for (const auto& neighbor : nearestNeighbors)
        kNeighbors.push_back(neighbor.second);

    return kNeighbors;
}