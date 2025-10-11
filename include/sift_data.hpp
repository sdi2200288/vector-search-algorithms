// //sift_data.hpp

// #ifndef SIFT_DATA_HPP
// #define SIFT_DATA_HPP

// #include <string>
// #include <vector>
// using namespace std;

// vector<vector<float>> read_sift_file(const string& filename);
// vector<vector<float>> return_sift_data();

// #endif

#ifndef SIFT_DATA_HPP
#define SIFT_DATA_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

vector<vector<float>> read_sift_file(const string& filename);
vector<vector<float>> return_sift_data();

vector<vector<float>> read_sift_file(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "❌ Error: Cannot open file " << filename << endl;
        return vector<vector<float>>();
    }

    vector<vector<float>> data;
    int d = 0;

    try {
        while (file.read((char*)&d, sizeof(int))) {
            if (d != 128) {
                cerr << "⚠️ Unexpected dimension: " << d << endl;
                break;
            }

            vector<float> vec(d);
            if (file.read((char*)vec.data(), d * sizeof(float))) {
                data.push_back(vec);
            } else {
                break;
            }
            
            // Περιορισμός για testing
            if (data.size() >= 1000) break;
        }
    } catch (const exception& e) {
        cerr << "Exception reading file: " << e.what() << endl;
    }

    cout << "✅ Loaded " << data.size() << " SIFT vectors" << endl;
    return data;
}

vector<vector<float>> return_sift_data() {
    return read_sift_file("../../sift_data/sift_base.fvecs");
}

#endif