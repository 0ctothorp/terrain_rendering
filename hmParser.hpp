#pragma once

#include <string>
#include <vector>

using namespace std;

class HMParser {
private:
    vector<short> data;
    const int width = 1201;
public:
    HMParser(const string &hmPath);
    void ParseHM(const string &hmPath);
    vector<short>* GetDataPtr();
};