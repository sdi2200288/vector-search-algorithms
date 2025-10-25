/* SIFT */

#include "../include/sift_data.hpp"
#include <iostream>
#include <fstream>

using namespace std;

using std::cerr;
using std::cout;
using std::endl;

vector<vector<float>> read_sift_file(const string& filename){
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "❌ Error: Cannot open file " << filename << endl;
        return {};
    }

    vector<vector<float>> data;
    int32_t d = 0;

    while (file.read(reinterpret_cast<char*>(&d), sizeof(int32_t))){
        if (d != 128) {
            cerr << "⚠️ Unexpected dimension: " << d << endl;
            break;
        }
        vector<float> vec(d);
        if (!file.read(reinterpret_cast<char*>(vec.data()), d * sizeof(float))){
            // truncated vector at end-of-file
            break;
        }
        data.push_back(move(vec));
        // Για testing: όριο
        if (data.size() >= 1000) break;
    }

    cout << "✅ Loaded " << data.size() << " SIFT vectors" << endl;
    return data;
}

std::vector<std::vector<float>> return_sift_data(const string& input_file) {
    return read_sift_file(input_file); //"../sift/sift_base.fvecs"
}

vector<vector<float>> read_sift_query(const string& filename){
    return read_sift_file(filename);
}

std::vector<std::vector<float>> return_sift_queries(const string& query_file) {
    return read_sift_query(query_file); //"../sift/sift_query.fvecs"
}



