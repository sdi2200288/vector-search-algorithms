#include <unistd.h>
// #include <iostream>
#include <cstring>

#include "hypercube.hpp"
#include "../include/sift_data.hpp"
#include "../include/mnist_data.hpp"


// using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage example:\n"
             << "./search -d <input file> -q <query file> -kproj <int> -w <double> "
             << "-M <int> -probes <int> -o <output file> -N <number of nearest> "
             << "-R <radius> -type <flag> -range <true|false> -hypercube\n";
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
    int kproj = 14, M = 10, N = 1, probes = 2, seed = 1;
    double w = 4.0, R;
    bool range = false, R_flag = false;

    if(use_hypercube){
        cout << "\n>>> Running Hypercube Algorithm...\n";
        for(int i = 1; i<argc; i++){
            string arg = argv[i];
            if(arg == "-d") input_file = argv[++i];
            else if(arg == "-q") query_file = argv[++i];
            else if(arg == "-kproj") kproj = stoi(argv[++i]);
            else if(arg == "-w") w = stod(argv[++i]);
            else if(arg == "-M") M = stoi(argv[++i]);
            else if(arg == "-probes") probes = stoi(argv[++i]); 
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
            cerr<<"Unknown type"<<endl;
            return 1;
        }

        // Έλεγχος data
         if(!dataset.empty()){
            cout << "Data size: " << dataset.size() << " vectors x " << dataset[0].size() << " dimensions" << endl;
         }

        Hypercube hypercube(seed, input_file, query_file, output_file, kproj, M, w, N, R, probes, type, range);
        hypercube.print_params();
        cout << endl;


        // vector<vector<double>> test_dataset = {
        //     {1.0, 2.0, 0.5, 1.5, 2.5, 0.1, 1.8, 2.2, 0.8, 1.2},
        //     {2.0, 3.0, 1.5, 2.5, 3.5, 1.1, 2.8, 3.2, 1.8, 2.2},
        //     {0.5, 1.0, 0.2, 0.8, 1.2, 0.05, 0.9, 1.1, 0.4, 0.6},
        //     {3.0, 4.0, 2.5, 3.5, 4.5, 2.1, 3.8, 4.2, 2.8, 3.2}
        // };
        // vector<vector<double>> test_query = {{1.0, 2.0, 0.5, 1.5, 2.5, 0.1, 1.8, 2.2, 0.8, 1.2}, {3.0, 4.0, 2.5, 3.5, 4.5, 2.1, 3.8, 4.2, 2.8, 3.2}};
        hypercube.build_index(dataset);
        // hypercube.Queries(queries, dataset);
        // cout << "aaaaaaa" << endl;
        hypercube.hypercube_func(dataset, queries);

    }
    else if (use_ivfflat) {
        cout << "\n>>> Running IVFFlat Algorithm...\n";
    }
    else {
        cerr << "Error: You must specify an algorithm flag (-lsh, -hypercube, or -ivfflat)\n";
    }

    return 0;
}

