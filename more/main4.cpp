#include <iostream>
#include <unistd.h>
#include <vector>

#include "ivfpq.hpp"
#include "../include/sift_data.hpp"
#include "../include/mnist_data.hpp"

using namespace std;

int main(int argc, char* argv[]){
    if(argc < 2){
        cerr << "Usage example:\n"
             << "./search -d <input file> -q <query file> -kclusters <int> -nprobe <int> "
             << "-M <int> -o <output file> -N <number of nearest> -R <radius> -type <flag> "
             << "-nbits <int> -range <true|false> -ivfpq -seed <int>\n";
        return 1;
    }  

    bool use_ivfpq = false;

    for(int i=0; i<argc; i++){
        string arg = argv[i];
        if(arg == "-ivfpq"){
            use_ivfpq = true;
            break;
        }
    }

    string input_file, query_file, output_file, type;
    int kclusters=50, nprobe=5, M=16, N=1, seed=1, nbits=8;
    double R;
    bool range, R_flag;

    if(use_ivfpq){
        cout << "\n>>> Running IVFPQ Algorithm... \n";
        for(int i=1; i<argc; i++){
            string arg = argv[i];
            if(arg == "-d") input_file = argv[++i];
            else if(arg == "-q") query_file = argv[++i];
            else if(arg == "-kclusters") kclusters = stoi(argv[++i]);
            else if(arg == "-nprobe") nprobe = stoi(argv[++i]);
            else if(arg == "-M") M = stoi(argv[++i]);
            else if(arg == "-o") output_file = argv[++i];
            else if(arg == "-N") N = stoi(argv[++i]);
            else if(arg == "-R"){ R_flag = true; R = stod(argv[++i]);}
            else if(arg == "-type") type = argv[++i];
            else if(arg == "-nbits") nbits = stoi(argv[i+1]);
            else if (arg == "-range"){
                string s = argv[++i];
                range = (s == "true");
            }
            else if(arg == "-seed") seed = stoi(argv[i+1]);
        }

        if(!R_flag){
            if(type == "-mnist") R = 2000.0;
            else if(type == "-sift") R = 2.0;
        }

        vector<vector<double>> dataset;
        vector<vector<double>> queries;

        if(type == "mnist"){
            cout << "MNIST: " << endl;
            vector<vector<float>> float_data = return_mnist_data();
            vector<vector<float>> float_queries = return_mnist_queries();
            
            dataset.resize(float_data.size());
            for(size_t i=0; i<float_data.size(); i++){
                dataset[i].assign(float_data[i].begin(), float_data[i].end());
            }

            queries.resize(float_queries.size());
            for(size_t i=0; i<float_queries.size(); i++){
                queries[i].assign(float_queries[i].begin(), float_queries[i].end());
            }
        }
        else if(type == "sift"){
            cout << "SIFT: " << endl;
            vector<vector<float>> float_data = return_sift_data();
            vector<vector<float>> float_queries = return_sift_queries();
            
            dataset.resize(float_data.size());
            for(size_t i=0; i<float_data.size(); i++){
                dataset[i].assign(float_data[i].begin(), float_data[i].end());
            }

            queries.resize(float_queries.size());
            for(size_t i=0; i<float_queries.size(); i++){
                queries[i].assign(float_queries[i].begin(), float_queries[i].end());
            }
        }
        else{
            cerr << "Unknown type" << endl;
            return 1;
        }

        if(!dataset.empty()){
            cout << "Data size: " << dataset.size() << " vectors x " << dataset[0].size() << " dimensions" << endl;
        }

        IVFPQ ivfpq(input_file, query_file, output_file, type, kclusters, nprobe, M, N, seed, nbits, R, range);
        ivfpq.print_params();
        cout << endl; 

        ivfpq.ivfpq_func(dataset, queries);
    }
    else{
        cerr << "Error: You must specify an algorithm flag (-lsh, -hypercube, or -ivfflat)\n";
    }

    return 0;
}