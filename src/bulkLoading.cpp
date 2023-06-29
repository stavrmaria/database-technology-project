#include <iostream>
#include <bitset>
#include <vector>

using namespace std;

string doubleToBinaryString(double value) {
    // Create a union of a double and an unsigned long long
    union {
        double d;
        unsigned long long u;
    } converter;
    converter.d = value;
    unsigned long long binary = converter.u;

    // Convert the binary value to a binary string
    bitset<64> binaryBits(binary);
    string binaryString = binaryBits.to_string();
    return binaryString;
}

string zOrderValue(vector<double> coordinates) {
    string value = "";
    vector<string> binaryRepresentation;

    for (const auto &x : coordinates) {
        string binString = doubleToBinaryString(x);
        binaryRepresentation.push_back(binString);
    }

    int n = binaryRepresentation.at(0).size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < coordinates.size(); j++) {
            value += binaryRepresentation.at(j)[i];
        }
    }

    return value;
}