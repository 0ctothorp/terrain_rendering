#include <fstream>
#include <iostream>

#include "HMParser.hpp"

HMParser::HMParser(const string &hmPath) {
    data.resize(1201);
    for(int i = 0; i < data.size(); i++)
        data[i].resize(1201);

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
        int i;
        for(i = 0; i < 1201; i++) {
            hmFile.read(reinterpret_cast<char*>(data[i].data()), sizeof(short) * 1201);
            for(int j = 0; j < 1201; j++) {
                short swapped = 0;
                swapped = data[i][j] << 8;
                swapped |= data[i][j] >> 8;
                data[i][j] = swapped;
            }
        }

        hmFile.close();
    } catch(const ifstream::failure &e) {
        cerr << "[EXCEPTION: HMParser::HMParser] " << e.what() << '\n'
             << "Error code: " << e.code() << '\n';
        exit(EXIT_FAILURE);
    }
}

vector< vector<short> >* HMParser::GetDataPtr() {
    return &data;
}