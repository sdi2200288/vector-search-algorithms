#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "sift.hpp"

using namespace std;

vector<vector<float>> read_sift_file(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "❌ Error: Cannot open file " << filename << endl;
        exit(1);
    }

    vector<vector<float>> data;
    int d; // dimension (should be 128)

    while (file.read((char*)&d, sizeof(int))) {
        if (d != 128) {
            cerr << "⚠️ Unexpected dimension value: " << d << endl;
            break;
        }

        vector<float> vec(d);
        file.read((char*)vec.data(), d * sizeof(float));
        if (!file) break;

        data.push_back(vec);
    }

    cout << "✅ Loaded " << data.size() << " vectors of dimension " 
         << (data.empty() ? 0 : data[0].size()) << endl;

    return data;
}

int main() {
    string filename = "../sift/sift_base.fvecs";  // <-- το δικό σου αρχείο
    auto data = read_sift_file(filename);

    string filename1 = "../sift/sift_query.fvecs";
    auto data1 = read_sift_file(filename1);

    string filename2 = "train-images-idx3-ubyte.gz";
    auto data2 = read_sift_file(filename2);

    // cout << "First vector (first 10 coords): ";
    // for(int j =0; j <5; j++){
    // for (int i = 0; i < 10; ++i)
    //     cout << data[j][i] << " ";
    // cout << endl;}

    //  cout << "First query (first 10 coords): ";
    // for(int j=0; j<5; j++){
    //  for (int i = 0; i < 10; ++i)
    //     cout << data1[j][i] << " ";
    // cout << endl;}

    cout << "First query (first 10 coords): ";
    for(int j=0; j<5; j++){
     for (int i = 0; i < 10; ++i)
        cout << data2[j][i] << " ";
    cout << endl;}
}
