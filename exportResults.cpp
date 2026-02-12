#include "exportResults.hpp"
#include <fstream>
#include <iostream>
using namespace std;

struct Output{
    vector<int> partition;
    vector<pair<int,int>> g;
    vector<pair<int,int>> h;
};

void writeResultsToJson(const vector<Output>& results,
                        const string& filename) {

    ofstream out(filename);
    if (!out) {
        cerr << "Error opening file.\n";
        return;
    }

    out << "[";
    for (size_t i = 0; i < results.size(); ++i) {
        const Output& r = results[i];
        if (i) out << ",";

        out << "{\"partition\":[";
        for (size_t j = 0; j < r.partition.size(); ++j) {
            if (j) out << ",";
            out << r.partition[j];
        }
        out << "],\"G\":[";

        for (size_t j = 0; j < r.g.size(); ++j) {
            if (j) out << ",";
            out << "[" << r.g[j].first << "," << r.g[j].second << "]";
        }

        out << "],\"H\":[";

        for (size_t j = 0; j < r.h.size(); ++j) {
            if (j) out << ",";
            out << "[" << r.h[j].first << "," << r.h[j].second << "]";
        }

        out << "]}";
    }
    out << "]";
}
