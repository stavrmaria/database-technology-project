#include <unordered_set>
#include "RStarTree.h"

// Basic constructor for the R* Tree
newRStarTree::newRStarTree(int maxEntries, int dimensions, int maxObjectSize) {
    this->dimensions = dimensions;
    this->root = new Node(this->dimensions, true);
    this->maxEntries = maxEntries;
    this->minEntries = (int)round(maxEntries / 2);
    this->nodesCount = 1;
    this->maxObjectSize = maxObjectSize;
    this->levelCallMap = {};
}

// Recursively delete the R* Tree
newRStarTree::~newRStarTree() {
    if (root == nullptr) {
        levelCallMap.clear();
        return;
    }

    destroyNode(root);
    delete root;
    root = nullptr;
}

void newRStarTree::setRoot(Node *root) {
    this->root = root;
}

int newRStarTree::getDimensions() {
    return this->root->getDimensions();
}

Node *newRStarTree::getRoot() {
    return this->root;
}

void newRStarTree::destroyNode(Node* currentNode) {
    if (currentNode->isLeafNode()) {
        // Delete leaf node and its entries
        for (auto entry : currentNode->getEntries()) {
            delete entry;
        }

        currentNode->clearEntries();
    } else {
        // Recursively destroy child nodes
        for (auto entry : currentNode->getEntries()) {
            destroyNode(entry->childNode);
            delete entry;  // Move the delete statement here
        }
    }
}

/******************** Insertion Functions ********************/
// Insert a point into the R* Tree
void newRStarTree::insertData(Point &point, unsigned int &blockID, unsigned int &slot) {
    // Create the entry
    Entry *entry = new Entry();
    entry->childNode = nullptr;
    entry->boundingBox = new BoundingBox(this->dimensions, point.getCoordinates(), point.getCoordinates());
    entry->id = new ID;
    entry->id->blockID = blockID;
    entry->id->slot = slot;
    insert(entry, this->root, 0);
}

void newRStarTree::insert(Entry* entry, Node* currentNode, int level) {
    currentNode = chooseSubtree(entry, currentNode, level);
    currentNode->insertEntry(entry);
    Node* newNode;
    Node* parentNode = currentNode->getParent();

//     if (parentNode != nullptr && parentNode->findEntry(currentNode) == nullptr) {
//         return;
//     } else
    if (currentNode->entriesSize() <= this->maxEntries) {
        adjustTree(currentNode);
        return;
    }

    // There is no available space to place the point
    while (currentNode->entriesSize() > this->maxEntries) {
        // Overflow occurred, invoke OverflowTreatment with the level of the node
        newNode = overFlowTreatment(currentNode);
        if (newNode != nullptr) {
            if (currentNode->getParent() == nullptr) {
                // Split happened at the root, create a new root
                Node* newRoot = new Node(this->dimensions, false);
                newRoot->setLevel(currentNode->getLevel() + 1);
                createChildEntry(currentNode, newRoot);
                createChildEntry(newNode, newRoot);

                // Update the tree's root node
                this->root = newRoot;
                adjustTree(currentNode);
                adjustTree(newNode);
                break;
            } else {
                // Split happened at a non-root node
                parentNode = currentNode->getParent();
                createChildEntry(newNode, parentNode);
                adjustTree(newNode);

                // Move up to the parent node
                currentNode = parentNode;
            }
        }
    }
}

// Find the appropriate entry for a given point based on the MBBs
Node* newRStarTree::chooseSubtree(Entry* newEntry, Node* node, int level) {
    Node* currentNode = node;
    int currentLevel = node->getLevel();

    // Traverse the tree by selecting the entry whose rectangle needs
    // the least enlargement to include the new point
    while (currentLevel > level) {
        Entry* entryToChoose = nullptr;
        vector<Entry*> entries = currentNode->getEntries();

        // The child pointers in the current node point to leaves
        if (currentNode->getEntries().at(0)->childNode->isLeafNode()) {
            double minOverlapEnlargement;
            double minAreaEnlargement = numeric_limits<double>::infinity();

            // Find the entry in the current node whose rectangle needs least overlap enlargement
            // Resolve ties by choosing the entry whose rectangle needs least area enlargement
            entryToChoose = currentNode->minOverlapEntry(newEntry, minOverlapEnlargement);

            for (auto entry : entries) {
                BoundingBox originalEntryBB = *(entry->boundingBox);
                BoundingBox updatedEntryBB = *(entry->boundingBox);
                updatedEntryBB.includeBox(*(newEntry->boundingBox));
                double overlapEnlargement = originalEntryBB.calculateOverlap(*(newEntry->boundingBox));
                double areaEnlargement = updatedEntryBB.getArea() - originalEntryBB.getArea();

                if (abs(overlapEnlargement - minOverlapEnlargement) < ERROR && areaEnlargement < minAreaEnlargement) {
                    minAreaEnlargement = areaEnlargement;
                    entryToChoose = entry;
                }
            }
        } else {
            // The child pointers in the current node point to leaves, determine the minimum area cost
            // Resolve ties by choosing the entry with the rectangle of smallest area
            double minAreaEnlargement;
            entryToChoose = currentNode->minEnlargedAreaEntry(newEntry, minAreaEnlargement);
            for (auto entry : entries) {
                BoundingBox originalEntryBB = *(entry->boundingBox);
                BoundingBox updatedEntryBB = *(entry->boundingBox);
                updatedEntryBB.includeBox(*(newEntry->boundingBox));
                double areaEnlargement = updatedEntryBB.getArea() - originalEntryBB.getArea();

                if (abs(areaEnlargement - minAreaEnlargement) < ERROR && entry->boundingBox->getArea() < entryToChoose->boundingBox->getArea()) {
                    minAreaEnlargement = areaEnlargement;
                    entryToChoose = entry;
                }
            }
        }

        currentNode = entryToChoose->childNode;
        currentLevel--;
    }

    return currentNode;
}

Node *newRStarTree::overFlowTreatment(Node* currentNode) {
    if (currentNode->getParent() != nullptr && isFirstCallOfLevel(currentNode->getLevel())) {
        reInsert(currentNode);
        adjustTree(currentNode);
        return nullptr;
    }

    bool isLeaf = currentNode->isLeafNode();
    Node *newNode = new Node(this->dimensions, isLeaf);
    newNode->setLevel(currentNode->getLevel());
    splitNode(currentNode, newNode);
    return newNode;
}

// Check if the overflow treatment has been called before in the node's level
bool newRStarTree::isFirstCallOfLevel(int level) {
    auto it = find(this->levelCallMap.begin(), this->levelCallMap.end(), level);
    bool found = (it != this->levelCallMap.end());

    if (!found)
        this->levelCallMap.push_back(level);

    return !found;
}

void newRStarTree::reInsert(Node *currentNode) {
    // Calculate for each entry the distance between the entry itself and the current node's MBB
    vector<Entry*> entries = currentNode->getEntries();
    vector<double> distances;
    BoundingBox nodeBoundingBox(this->dimensions);

    for (const auto & entry : entries)
        nodeBoundingBox.includeBox(*(entry->boundingBox));

    Point nodeCenterPoint = nodeBoundingBox.getCenter();
    for (auto & entry : entries) {
        Point centerPoint = entry->boundingBox->getCenter();
        double distance = nodeCenterPoint.getDistance(centerPoint);
        distances.push_back(distance);
    }

    // Sort the entries based on the decreasing order of the distances
    sort(entries.begin(), entries.end(), [&](const Entry* x, const Entry* y) {
        int indexX = distance(entries.begin(), find(entries.begin(), entries.end(), x));
        int indexY = distance(entries.begin(), find(entries.begin(), entries.end(), y));
        return distances[indexX] > distances[indexY];
    });

    // Remove the first p entries from the current node
    int removedEntriesNum = int(REINSERTION_PER * (this->maxEntries + 1));
    vector<Entry*> removedEntries;
    copy(entries.begin(), entries.begin() + removedEntriesNum, back_inserter(removedEntries));
    entries.erase(entries.begin(), entries.begin() + removedEntriesNum);
    currentNode->clearEntries();

    for (const auto &entry: entries)
        currentNode->insertEntry(entry);

    // Reinsert the removed entries
    for (const auto &removedEntry: removedEntries)
        insert(removedEntry, this->root, currentNode->getLevel());

    removedEntries.clear();
    entries.clear();
}

void newRStarTree::splitNode(Node *currentNode, Node *newNode) {
    int selectedAxis = chooseSplitAxis(currentNode);
    int selectedSplitIndex = chooseSplitIndex(currentNode, selectedAxis);
    vector<Entry*> sortedEntries = currentNode->getEntries();
    sort(sortedEntries.begin(), sortedEntries.end(), [&selectedAxis](Entry *a, Entry *b) {
        return a->boundingBox->compareBoundingBox(*(b->boundingBox), selectedAxis);
    });

    currentNode->clearEntries();
    for (int i = 0; i < this->minEntries - 1 + selectedSplitIndex; i++)
        currentNode->insertEntry(sortedEntries.at(i));

    newNode->clearEntries();
    for (int i = this->minEntries - 1 + selectedSplitIndex; i < this->maxEntries + 1; i++)
        newNode->insertEntry(sortedEntries.at(i));

    // Update the parents as well
    if (!currentNode->isLeafNode() || !newNode->isLeafNode()) {
        for (int i = 0; i < this->minEntries - 1 + selectedSplitIndex; i++)
            sortedEntries.at(i)->childNode->setParent(currentNode);

        for (int i = this->minEntries - 1 + selectedSplitIndex; i < this->maxEntries + 1; i++)
            sortedEntries.at(i)->childNode->setParent(newNode);
    }

    sortedEntries.clear();
}

// Determine the axis, perpendicular to which the split is performed
int newRStarTree::chooseSplitAxis(Node *currentNode) const {
    double minS = numeric_limits<double>::infinity();
    int minSIndex = 0;

    for (int i = 0; i < this->dimensions; i++) {
        // Sort the entries by the lower and upper value of their rectangles for the current axis
        vector<Entry*> sortedEntries = currentNode->getEntries();
        sort(sortedEntries.begin(), sortedEntries.end(), [i](Entry *a, Entry *b) {
            return a->boundingBox->compareBoundingBox(*(b->boundingBox), i);
        });

        // Determine the distributions, where the first group constains m - 1 + k entries
        for (int k = this->minEntries; k < this->maxEntries - 2 * this->minEntries + 2; k++) {
            BoundingBox firstGroupBB(this->dimensions);
            BoundingBox secondGroupBB(this->dimensions);

            // Create the two groups of the MBBs
            for (int j = 0; j < this->minEntries - 1 + k; j++)
                firstGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
            for (int j = this->minEntries - 1 + k; j < this->maxEntries + 1; j++)
                secondGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));

            double marginValue = firstGroupBB.calculateMargin() + secondGroupBB.calculateMargin();
            // Update minimum S and split axis if necessary
            if (marginValue < minS) {
                minS = marginValue;
                minSIndex = i;
            }
        }

        sortedEntries.clear();
    }

    return minSIndex;
}

// Along the chosen split axis, choose the distribution with the minimum overlap-value
int newRStarTree::chooseSplitIndex(Node *currentNode,int &selectedAxis) const {
    // Sort the entries by the lower and upper value of their rectangles based on the selected axis
    vector<Entry*> sortedEntries = currentNode->getEntries();
    sort(sortedEntries.begin(), sortedEntries.end(), [&selectedAxis](Entry *a, Entry *b) {
        return a->boundingBox->compareBoundingBox(*(b->boundingBox), selectedAxis);
    });

    // Choose the distribution with the minimum overlap-value
    // Resolve ties by choosing the distribution with minimum area-value
    double minOverlap = numeric_limits<double>::infinity();
    double minArea = numeric_limits<double>::infinity();
    int minOverlapIndex = 0;

    for (int k = this->minEntries; k < this->maxEntries - 2 * this->minEntries + 2; k++) {
        BoundingBox firstGroupBB(this->dimensions);
        BoundingBox secondGroupBB(this->dimensions);

        // Create the two groups of the MBBs
        for (int j = 0; j < this->minEntries - 1 + k; j++)
            firstGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));
        for (int j = this->minEntries - 1 + k; j < this->maxEntries + 1; j++)
            secondGroupBB.includeBox(*(sortedEntries.at(j)->boundingBox));

        double overlapValue = firstGroupBB.calculateOverlap(secondGroupBB);
        double areaValue = firstGroupBB.getArea() + secondGroupBB.getArea();
        if (overlapValue < minOverlap) {
            minOverlap = overlapValue;
            minOverlapIndex = k;
        } else if (overlapValue == minOverlap && areaValue < minArea) {
            minArea = areaValue;
            minOverlapIndex = k;
        }
    }

    sortedEntries.clear();
    return minOverlapIndex;
}

void newRStarTree::adjustTree(Node *currentNode) {
    if (currentNode == nullptr)
        return;

    // While the current node is not the root, move upwards
    while (currentNode->getParent() != nullptr) {
        // Adjust the MBB of the parent entry so that it tightly encloses all entry rectangles in the current node
        Node *parentNode = currentNode->getParent();
        parentNode->setLevel(currentNode->getLevel() + 1);
        Entry *currentNodeEntry = nullptr;
        // Find the entry of the parent that contains the node
        for (const auto &parentEntry : parentNode->getEntries()) {
            if (parentEntry->childNode == currentNode) {
                currentNodeEntry = parentEntry;
                break;
            }
        }

        BoundingBox newBoundingBox(this->dimensions);

        for (const auto entry : currentNode->getEntries())
            newBoundingBox.includeBox(*entry->boundingBox);

        *(currentNodeEntry->boundingBox) = newBoundingBox;
        currentNode = parentNode;
    }
}

void createChildEntry(Node* currentNode, Node* parentNode) {
    if (parentNode->isLeafNode())
        return;

    int dimensions = currentNode->getDimensions();
    Entry* newEntry = parentNode->findEntry(currentNode);

    if (newEntry == nullptr) {
        // Entry doesn't exist, create a new one
        newEntry = new Entry();
        newEntry->childNode = currentNode;
        currentNode->setParent(parentNode);
        BoundingBox* newBoundingBox = new BoundingBox(dimensions);

        for (const auto entry : currentNode->getEntries())
            newBoundingBox->includeBox(*entry->boundingBox);

        newEntry->boundingBox = newBoundingBox;
        parentNode->insertEntry(newEntry);
    } else {
        // Entry already exists, update its bounding box
        BoundingBox* currentBoundingBox = new BoundingBox(dimensions);

        for (const auto entry : currentNode->getEntries())
            currentBoundingBox->includeBox(*entry->boundingBox);

        newEntry->boundingBox->includeBox(*currentBoundingBox);
        delete currentBoundingBox;
    }
}

// Display the R* Tree using DFS traversal
void newRStarTree::display() {
    if (this->root == nullptr) {
        return;
    }

    int pointsPerBlock = int(BLOCK_SIZE / maxObjectSize);
    stack<Node*> nodeStack;
    nodeStack.push(this->root);
    int count = 0;
    while (!nodeStack.empty()) {
        Node* current = nodeStack.top();
        nodeStack.pop();
        if (current->isLeafNode()) {
            for (auto entry : current->getEntries()) {
                unsigned long long id = findObjectById(*(entry->id), pointsPerBlock).getID();
                cout << "(" << id << ") ";
                count++;
            }
            cout << endl;
        }

        for (auto entry : current->getEntries()) {
            if (entry->childNode == nullptr)
                continue;
            nodeStack.push(entry->childNode);
        }
    }

    cout << "Points = " << count << endl;
}

/********************* Deletion Functions ********************/
void newRStarTree::deletePoint(Point& point) {
    pair<Entry*, Node*> pairToDelete = findEntryToDelete(point, this->root);
    if (pairToDelete.first != nullptr) {
        Node* leafNode = pairToDelete.second;
        leafNode->deleteEntry(pairToDelete.first);
        Node* parentNode = pairToDelete.second->getParent();
        // Adjust bounding boxes on the path to the root
        if (parentNode != nullptr)
            adjustTree(parentNode);
        condenseTree(pairToDelete.second);
    }
}

pair<Entry*, Node*> newRStarTree::findEntryToDelete(Point& point, Node* currentNode) {
    stack<Node*> nodeStack;
    nodeStack.push(currentNode);
    while (!nodeStack.empty()) {
        Node* current = nodeStack.top();
        nodeStack.pop();
        if (current->isLeafNode()) {
            for (auto entry : current->getEntries()) {
                if (entry->boundingBox->contains(point))
                    return make_pair(entry, current);
            }
        }

        for (auto entry : current->getEntries()) {
            if (entry->childNode == nullptr)
                continue;
            if (entry->boundingBox->contains(point))
                nodeStack.push(entry->childNode);
        }
    }

    return make_pair(nullptr, nullptr); // Entry not found
}

void newRStarTree::condenseTree(Node* leafNode) {
    Node* currentNode = leafNode;
    unordered_set<Node*> visitedNodes;

    while (currentNode->getParent() != nullptr) {
        if (currentNode->entriesSize() < this->minEntries) {
            Node* parentNode = currentNode->getParent();
            visitedNodes.insert(currentNode);

            if (currentNode->isLeafNode()) {
                parentNode->removeChild(currentNode);
            } else {
                adjustNonLeafNode(currentNode, parentNode);
            }

            currentNode = parentNode;
        } else {
            adjustTree(currentNode);
            currentNode = currentNode->getParent();
        }
    }

    // Check if the root has become empty and has only one child
    if (this->root->entriesSize() == 0 && !this->root->isLeafNode()) {
        Node* newRoot = new Node(this->dimensions, true);
        newRoot->setParent(nullptr);
        this->root = newRoot;
    }

}

void newRStarTree::adjustNonLeafNode(Node* currentNode, Node* parentNode) {
    parentNode->removeChild(currentNode);
    vector<Entry*> allEntries;

    // Collect all entries in the current node and its siblings
    for (auto siblingEntry : parentNode->getEntries()) {
        Node* siblingNode = siblingEntry->childNode;
        if (siblingNode != currentNode) {
            for (auto entry : siblingNode->getEntries()) {
                allEntries.push_back(entry);
            }
            siblingNode->clearEntries();
        }
    }

    // Reinsert all entries
    for (auto entry : currentNode->getEntries()) {
        allEntries.push_back(entry);
    }

    // Clear current node entries
    currentNode->clearEntries();

    // Reinsert entries to get better distribution
    for (auto entry : allEntries) {
        insert(entry, this->root, currentNode->getLevel());
    }
}