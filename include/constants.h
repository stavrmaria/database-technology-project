#ifndef CONSTANTS_H
#define CONSTANTS_H

// Define the block size and the file names
const string MAP_FILE = "map.osm";
const string CSV_FILE = "input.csv";
const string DATA_FILE = "datafile.dat";
const string INDEX_FILE = "indexfile.bin";
const string SORTED_CSV_FILE = "sorted_input.csv";
const vector<string> attributeNames = {"id", "name", "lat", "lon"};

#define BLOCK_SIZE 1 * 1024
#define REINSERTION_PER 0.3
#define ERROR 0.0001

struct ID;

void writeToCSV(const string& csvFileName, const string& dataFileName, const vector<string>& attributeNames, unsigned int &pointCount, bool &sortFirst);
Point findObjectById(ID id, int &maxObjectSize);
Point parsePoint(string line);
void externalSort(string inputFileName, string outputFileName, int numWays, int runSize);

#endif
