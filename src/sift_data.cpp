/* sift_data.cpp */

#include <iostream>
#include <fstream>

#include "../include/sift_data.hpp"

using namespace std;

using std::cerr;
using std::cout;
using std::endl;

//βασική συνάρτηση που κάνει ανάγνωση τα sift αρχεία
vector<vector<float>> read_sift_file(const string& filename){
    ifstream file(filename, ios::binary);
    if(!file.is_open()){
        cerr << "Error: Cannot open file " << filename << endl;
        return {}; // επιστροφή κενού vector αντί για crash
    }

    vector<vector<float>> data;
    int32_t d = 0;

    //ανάγνωση binary format με έλεγχο ορθότητας
    while (file.read(reinterpret_cast<char*>(&d), sizeof(int32_t))){
        if (d != 128) { // έλεγχος ότι η διάσταση είναι 128 
            cerr << "Unexpected dimension: " << d << endl;
            break;
        }

        vector<float> vec(d);
        
        //έλεγχος για truncated file
        if (!file.read(reinterpret_cast<char*>(vec.data()), d * sizeof(float)))
            break;  //truncated vector at end-of-file

        data.push_back(move(vec));
        //για testing: όριο
        if (data.size() >= 1000) break;
    }

    // cout << "Loaded " << data.size() << " SIFT vectors" << endl;
    return data;
}

//συνάρτηση για ανάγνωση sift training/test data
vector<vector<float>> return_sift_data(const string& input_file) {
    return read_sift_file(input_file);      //"../sift/sift_base.fvecs"
}

//συνάρτηση για ανάγνωση sift queries
vector<vector<float>> read_sift_query(const string& filename){
    return read_sift_file(filename);
}

//συνάρτηση για επιστροφή sift queries
vector<vector<float>> return_sift_queries(const string& query_file) {
    return read_sift_query(query_file);     //"../sift/sift_query.fvecs"
}



