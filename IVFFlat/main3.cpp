#include <unistd.h>
// #include <iostream>
#include <cstring>

#include "IVFFlat.hpp"
#include "../include/sift_data.hpp"
#include "../include/mnist_data.hpp"


// using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage example:\n"
             << "./search -d input.dat -q query.dat -k 4 -L 5 -w 4.0 "
             << "-o output.txt -N 1 -R 2000 -type mnist -range true -lsh\n";
        return 1;
    }

    // Flags για ποιον αλγόριθμο θα τρέξει
    bool use_hypercube = false;
    bool use_ivfflat = false;

    for(int i=0; i<argc; i++){
        string arg = argv[i];        
        if(arg == "-hypercube"){
            use_hypercube = true;
            break;
        }
        else if(arg == "-ivfflat"){
            use_ivfflat = true;
            break;
        }
    }

    string input_file, query_file, output_file, type;
    int kclusters = 14, N = 1, nprobe = 2, seed = 1;
    double R;
    bool range = false, R_flag = false;

    if(use_ivfflat){
        cout << "\n>>> Running IVFFlat Algorithm...\n";
        for(int i = 1; i<argc; i++){
            string arg = argv[i];
            if(arg == "-d") input_file = argv[++i];
            else if(arg == "-q") query_file = argv[++i];
            else if(arg == "-kclusters") kclusters = stoi(argv[++i]);
            else if(arg == "-nprobe") nprobe = stoi(argv[++i]); 
            else if(arg == "-o") output_file = argv[++i];
            else if(arg == "-N") N = stoi(argv[++i]);
            else if(arg == "-R"){ R_flag = true; R = stod(argv[++i]);}
            else if(arg == "-type") type = argv[++i];
            else if (arg == "-range") {
                string s = argv[++i];
                range = (s == "true");
            }
        }
        cout << "R_flag: " << R_flag << endl;
        if(!R_flag){
            if(type == "mnist") R = 2000.0;
            else if(type == "sift") R = 2.0;
        }

        vector<vector<double>> dataset;
        vector<vector<double>> queries;
       
        if(type == "mnist"){
            cout << "MNIST: " << endl;
            vector<vector<float>> float_data = return_mnist_data();
            vector<vector<float>> float_queries = return_mnist_queries();
            
            dataset.resize(float_data.size());
            for(size_t i = 0; i < float_data.size(); i++) {
                dataset[i].assign(float_data[i].begin(), float_data[i].end());
            }
            queries.resize(float_queries.size());
            for(size_t i = 0; i < float_queries.size(); i++) {
                queries[i].assign(float_queries[i].begin(), float_queries[i].end());
            }
            
        }
        else if(type == "sift"){
            cout << "SIFT: " << endl;
            vector<vector<float>> float_data = return_sift_data();
            vector<vector<float>> float_queries = return_sift_queries();
            
            dataset.resize(float_data.size());
            for(size_t i = 0; i < float_data.size(); i++) {
                dataset[i].assign(float_data[i].begin(), float_data[i].end());
            }
            queries.resize(float_queries.size());
            for(size_t i = 0; i < float_queries.size(); i++) {
                queries[i].assign(float_queries[i].begin(), float_queries[i].end());
            }
        }
        else{
            cerr<<"Unkown type"<<endl;
            return 1;
        }

        // Έλεγχος data
        if(!dataset.empty()){
            cout << "Data size: " << dataset.size() << " vectors x " << dataset[0].size() << " dimensions" << endl;
        }


    }
    else if (use_ivfflat) {
        cout << "\n>>> Running IVFFlat Algorithm...\n";
    }
    else {
        cerr << "Error: You must specify an algorithm flag (-lsh, -hypercube, or -ivfflat)\n";
    }

    return 0;
}

