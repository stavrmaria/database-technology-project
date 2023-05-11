#ifndef CONSTANTS_H
#define CONSTANTS_H

// Define the block size and the file names
const string MAP_FILE = "map.osm";
const string DATA_FILE = "datafile.dat";
const string INDEX_FILE = "indexfile.bin";
const vector<string> attributeNames = {"id", "lat", "lon","user"};

#define BLOCK_SIZE 1024

Point findObjectById(unsigned long &id);
Point parsePoint(string line);

#endif
