#include <iostream>
#include <cstring>
#include "../include/lsh.hpp"
#include "../include/sift_data.hpp"
#include "../include/mnist_data.hpp"
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage example:\n"
             << "./search -d input.dat -q query.dat -k 4 -L 5 -w 4.0 "
             << "-o output.txt -N 1 -R 2000 -type mnist -range true -lsh\n";
        return 1;
    }

    string input_file, query_file, output_file, type;
    int k = 4, L = 5, N = 1, seed = 1;
    double w = 4.0, R = 2000.0;
    bool range = false;

    // Flags για ποιον αλγόριθμο θα τρέξει
    bool use_lsh = false;
    bool use_hypercube = false;
    bool use_ivfflat = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "-d") input_file = argv[++i];
        else if (arg == "-q") query_file = argv[++i];
        else if (arg == "-o") output_file = argv[++i];
        else if (arg == "-k") k = stoi(argv[++i]);
        else if (arg == "-L") L = stoi(argv[++i]);
        else if (arg == "-w") w = stod(argv[++i]);
        else if (arg == "-N") N = stoi(argv[++i]);
        else if (arg == "-R") R = stod(argv[++i]);
        else if (arg == "-type") type = argv[++i];
        else if (arg == "-range") {
            string s = argv[++i];
            range = (s == "true");
        }
        //έλεγχος ποιο flag δόθηκε
        else if (arg == "-lsh") use_lsh = true;
        else if (arg == "-hypercube") use_hypercube = true;
        else if (arg == "-ivfflat") use_ivfflat = true;
    }

    if (use_lsh) {
        cout << "\n>>> Running LSH Algorithm...\n";

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
            
        }else if(type == "sift"){
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

        lsh LSH(seed, input_file, query_file, output_file, k, L, w, N, R, type, range);
        LSH.print_params();
        //LSH.Initialize(dataset.size());
        LSH.lsh_func(dataset,queries);
        //LSH.lsh_func( {1, 5 },{2,3},1);
    }
    else if (use_hypercube) {
        cout << "\n>>> Running Hypercube Algorithm...\n";
    }
    else if (use_ivfflat) {
        cout << "\n>>> Running IVFFlat Algorithm...\n";
    }
    else {
        cerr << "Error: You must specify an algorithm flag (-lsh, -hypercube, or -ivfflat)\n";
    }

    return 0;
}

