#include <iostream>
#include <fstream>
#include "../include/sift_data.hpp"
// #include <vector>
// #include <string>
// using namespace std;

// =============================================================
// 📘 SIFT Loader (fvecs format - Little Endian, float32)
// =============================================================
vector<vector<float>> read_sift_file(const string& filename){
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "❌ Error: Cannot open file " << filename << endl;
        exit(1);
    }

    vector<vector<float>> data;
    int d = 0; // dimension (should be 128)

    while (file.read((char*)&d, sizeof(int))) {
        if (d != 128) {
            cerr << "⚠️ Unexpected dimension value: " << d << " (expected 128)" << endl;
            break;
        }

        vector<float> vec(d);
        file.read((char*)vec.data(), d * sizeof(float));
        if (!file) break;
        data.push_back(vec);
    }

    cout << "✅ Loaded " << data.size() << " SIFT vectors of dimension "
         << (data.empty() ? 0 : data[0].size()) << endl;
    return data;
}

// int main() {
//     auto sift_data = read_sift_file("sift_data/sift_base.fvecs");
//     auto sift_queries = read_sift_file("sift_data/sift_query.fvecs");
    
//     cout << "SIFT base: " << sift_data.size() << " × " << sift_data[0].size() << endl;
//     cout << "SIFT queries: " << sift_queries.size() << " × " << sift_queries[0].size() << endl;

//     cout << "\nΠρώτο SIFT διάνυσμα (πρώτα 10 στοιχεία): ";
//     for (int i = 0; i < 128; ++i)
//         cout << sift_data[0][i] << " ";
//     cout << endl;
// }

vector<float> return_sift_data(){
    auto sift_data = read_sift_file("../../sift_data/sift_base.fvecs");
    auto sift_queries = read_sift_file("../../sift_data/sift_query.fvecs");
    
    cout << "SIFT base: " << sift_data.size() << " × " << sift_data[0].size() << endl;
    cout << "SIFT queries: " << sift_queries.size() << " × " << sift_queries[0].size() << endl;

    // cout << "\nΠρώτο SIFT διάνυσμα (πρώτα 10 στοιχεία): ";
    // for (int i = 0; i < 128; ++i)
    //     cout << sift_data[0][i] << " ";
    // cout << endl;

    // return mnist_data[0];
    // for(int i=0; i<mnist_data.size(); i++){
        for(int j=0; j<sift_data[0].size(); ++j){
            cout << sift_data[0][j] << " ";
        }
        cout << endl;
    // }

}

