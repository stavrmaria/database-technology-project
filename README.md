# Database Technology Project
## Introduction
In this project we deal with the organization and processing of spatial data (spatial data). Spatial data in their simplest form are points in a multidimensional space, such as for example points in the plane or in three-dimensional space. However, we can easily create points in spaces with more dimensions. In practice, the multitude of dimensions is a parameter that is taken into account when organizing the data and when processing queries.

## Structure
The implemented structure is a R*-tree and is an improvement of the R-tree. The data is from OpenStreetMap which allows us to download regions of the world with points of interest.  
The records are saved in a file (datafile.dat) which will consist of blocks of size B = 32KB. Each block contains a set of records and also has a unique  blockid.  
The directory (index) is also stored in a separate file and it organizes the records stored in the datafile. Therefore, in the catalog leaves the coordinates of the point are stored with a Record ID that essentially points to which block of the datafile, the specific record is stored and in which slot.  
The R*-tree supports the following functions:
 - Insertion
 - Deletion
 - Range query
 - k-nn query
 - Bottom-up tree construction

## Note
- Each block has a size of 1KB = 1024 bytes
- main.cpp creates 2 files indexfile and datafile

## Todo List
- [X] Insert
- [ ] Change entries (save only the coordinates)
- [ ] Delete
- [ ] Skyline queries
- [ ] Support range queries
- [ ] Support k-nn queries

### Index (Sample)
```
#include <fstream>
#include <vector>

// Define a class to represent a rectangular bounding box
class Rect {
public:
    float x1, y1, x2, y2;
};

// Define a class to represent an object in the index
class Object {
public:
    int id;
    Rect bbox;
};

// Define a class to represent a node in the R-tree
class RTreeNode {
public:
    Rect bbox;
    bool is_leaf;
    std::vector<Object> objects;
    std::vector<RTreeNode> children;
};

// Define a function to serialize an R-tree node and write it to a file
void serialize_node(const RTreeNode& node, std::ofstream& outfile) {
    // Write the node's bounding box to the file
    outfile.write(reinterpret_cast<const char*>(&node.bbox), sizeof(Rect));
    // Write a flag indicating whether the node is a leaf or not
    outfile.write(reinterpret_cast<const char*>(&node.is_leaf), sizeof(bool));
    // Write the number of objects in the node to the file
    int num_objects = node.objects.size();
    outfile.write(reinterpret_cast<const char*>(&num_objects), sizeof(int));
    // Write each object to the file
    for (const auto& obj : node.objects) {
        outfile.write(reinterpret_cast<const char*>(&obj), sizeof(Object));
    }
    // Write the number of children in the node to the file
    int num_children = node.children.size();
    outfile.write(reinterpret_cast<const char*>(&num_children), sizeof(int));
    // Recursively serialize each child node and write it to the file
    for (const auto& child : node.children) {
        serialize_node(child, outfile);
    }
}

// Define a function to build an R-tree index and save it to a file
void build_rtree(const std::vector<Object>& objects, const std::string& index_filename) {
    // Build the R-tree data structure using a recursive algorithm
    RTreeNode root_node;
    // ...
    // Populate the R-tree data structure
    // ...
    // Write the R-tree data structure to a file
    std::ofstream outfile(index_filename, std::ios::binary);
    serialize_node(root_node, outfile);
}

int main() {
    // Define some objects to index
    std::vector<Object> objects;
    // ...
    // Populate the objects vector
    // ...
    // Build the R-tree index and save it to a file
    build_rtree(objects, "index.dat");
    return 0;
}
```