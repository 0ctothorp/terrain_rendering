#pragma once

#include <string>
#include <vector>

using namespace std;

class HMParser {
private:
    vector< vector<short> > data;
public:
    HMParser(const string &hmPath);
    void ParseHM(const string &hmPath);
    vector< vector<short> >* GetDataPtr();
};