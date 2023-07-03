#include <iostream>
#include <bitset>
#include <vector>
#include <cmath>
#include <bits/stdc++.h>
#include <cstdio>

#include "Point.h"
#include "constants.h"

using namespace std;

struct MinHeapNode {
    Point element;
    int i;
};

void swap(MinHeapNode* x, MinHeapNode* y) {
    MinHeapNode temp = *x;
    *x = *y;
    *y = temp;
}

class MinHeap {
    MinHeapNode* harr;
    int heap_size;
public:
    MinHeap(MinHeapNode a[], int size);
    void MinHeapify(int);
    int left(int i) { return (2 * i + 1); }
    int right(int i) { return (2 * i + 2); }
    MinHeapNode getMin() { return harr[0]; }
    void replaceMin(MinHeapNode x) {
        harr[0] = x;
        MinHeapify(0);
    }

    // Compare two points based on their zOrderValue
    bool comparePoints(const Point& p1, const Point& p2) {
        unsigned long long z1 = p1.zOrderValue();
        unsigned long long z2 = p2.zOrderValue();
        return z1 < z2;
    }
};

MinHeap::MinHeap(MinHeapNode a[], int size) {
    heap_size = size;
    harr = a; // store the address of the array
    int i = (heap_size - 1) / 2;
    while (i >= 0) {
        MinHeapify(i);
        i--;
    }
}

void MinHeap::MinHeapify(int i) {
    int l = left(i);
    int r = right(i);
    int smallest = i;

    if (l < heap_size && comparePoints(harr[l].element, harr[i].element))
        smallest = l;

    if (r < heap_size && comparePoints(harr[r].element, harr[smallest].element))
        smallest = r;

    if (smallest != i) {
        swap(&harr[i], &harr[smallest]);
        MinHeapify(smallest);
    }
}

void merge(Point arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
    Point L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i].zOrderValue() <= R[j].zOrderValue())
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];
}

void mergeSort(Point arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}

void createInitialRuns(string inputFileName, int runSize, int numWays) {
    fstream in(inputFileName, ios::in);
    int fileNameLength = log10(numWays) + 1;
    vector<std::fstream> out(numWays);

    // Open output files in write mode.
    for (int i = 0; i < numWays; i++) {
        char fileName[fileNameLength + 1];
        snprintf(fileName, sizeof(fileName), "%d", i);
        out[i].open(fileName, std::ios::out);
        if (!out[i].is_open()) {
            std::cout << "Error with the creation of scratch file." << std::endl;
            return;
        }
    }

    Point* arr = new Point[runSize];
    bool moreInput = true;
    int nextOutputFile = 0;
    int i;
    string line;

    while (moreInput) {
        // write runSize elements into arr from the input file
        for (i = 0; i < runSize; i++) {
            if (!getline(in, line)) {
                moreInput = false;
                break;
            }
            // Parse the point from the line
            Point point = parsePoint(line);
            arr[i] = point;
        }

        // sort array using merge sort
        mergeSort(arr, 0, i - 1);
        // write the records to the appropriate scratch output file
        for (int j = 0; j < i; j++) {
            out[nextOutputFile] << arr[j].toString() << endl;
            nextOutputFile = (nextOutputFile + 1) % numWays;
        }
    }

    // close input and output files
    for (int i = 0; i < numWays; i++)
        out[i].close();
    in.close();

    // deallocate the memory for the arr array
    delete[] arr;
}


void mergeFiles(string outputFile, int n, int k) {
    vector<ifstream> in(k);
    for (int i = 0; i < k; i++) {
        string fileName = to_string(i);
        in[i].open(fileName);
    }

    fstream out(outputFile, ios::out);
    MinHeapNode* harr = new MinHeapNode[k];
    int i;
    for (i = 0; i < k; i++) {
        string line;
        if (getline(in[i], line)) {
            stringstream ss(line);
            ss >> harr[i].element;
            harr[i].i = i;
        } else {
            break;
        }
    }

    MinHeap hp(harr, i);
    int count = 0;
    while (count != i) {
        MinHeapNode root = hp.getMin();
        out << root.element << endl;
        string line;
        if (getline(in[root.i], line)) {
            stringstream ss(line);
            ss >> root.element;
        } else {
            root.element.setDimension(0, std::numeric_limits<double>::max());
            count++;
        }

        hp.replaceMin(root);
    }

    for (int i = 0; i < k; i++) {
        in[i].close();
        remove(to_string(i).c_str());
    }

    out.close();
}

// For sorting data stored on disk
void externalSort(string inputFileName, string outputFileName, int numWays, int runSize) {
    createInitialRuns(inputFileName, runSize, numWays);
    mergeFiles(outputFileName, runSize, numWays);
}
