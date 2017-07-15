#include <fstream>
#include <iostream>

#include "hmParser.hpp"

HMParser::HMParser(const string &hmPath) {
    data.resize(width * width);
    ParseHM(hmPath);
}

void HMParser::ParseHM(const string &hmPath) {
    ifstream hmFile;
    hmFile.exceptions(ifstream::badbit | ifstream::failbit);
    try {
        hmFile.open(hmPath, ios_base::in | ios_base::binary);
        // - SRTM3 .hgt file has 1201 rows and 1201 samples (in each row)
        // - big-endian 16-bit signed integers - need to byte-swap
        // - height data every 3 arc-seconds in both directions
        hmFile.read(reinterpret_cast<char*>(data.data()), sizeof(short) * width * width);
        hmFile.close();
    } catch(const ifstream::failure &e) {
        cerr << "[EXCEPTION: HMParser::ParseHM] " << e.what() << '\n'
             << "Error code: " << e.code() << '\n';
        exit(EXIT_FAILURE);
    }
}

vector<short>* HMParser::GetDataPtr() {
    return &data;
}