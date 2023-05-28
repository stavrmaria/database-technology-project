#ifndef CONSTANTS_H
#define CONSTANTS_H

// Define the block size and the file names
const string MAP_FILE = "map.osm";
const string CSV_FILE = "input.csv";
const string DATA_FILE = "datafile.dat";
const string INDEX_FILE = "indexfile.bin";
const vector<string> attributeNames = {"id", "name", "lat", "lon"};

#define BLOCK_SIZE 1024
#define REINSERTION_PER 0.3

void writeToCSV(const string& csvFileName, const string& dataFileName, const vector<string>& attributeNames, unsigned int &pointCount);
Point findObjectById(ID id, int &maxObjectSize);
Point parsePoint(string line);

#endif
