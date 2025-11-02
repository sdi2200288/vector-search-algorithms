/* main */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <vector>

#include "../include/lsh.hpp"
#include "../include/hypercube.hpp"
#include "../include/IVFFlat.hpp"
#include "../include/ivfpq.hpp"
#include "../include/sift_data.hpp"
#include "../include/mnist_data.hpp"

using namespace std;

int main(int argc, char* argv[]){
    // Flags για το ποιός αλγόριθμος θα τρέξει (μονο ενας μπορει να ειναι true καθε φορα)
    bool use_lsh = false;
    bool use_hypercube = false; 
    bool use_ivfflat = false;
    bool use_ivfpq = false;

    for(int i=0; i<argc; i++){
        string arg = argv[i];
        if(arg == "-lsh"){
            use_lsh = true;
            break;
        }        
        else if(arg == "-hypercube"){
            use_hypercube = true;
            break;
        }
        else if(arg == "-ivfflat"){
            use_ivfflat = true;
            break;
        }
        else if(arg == "-ivfpq"){
            use_ivfpq = true;
            break;
        } 
    }

    string type, input_file, query_file, output_file;
    for(int i=0; i<argc; i++){
        string arg = argv[i];
        if (arg == "-d") input_file = argv[++i];
        else if (arg == "-q") query_file = argv[++i];
        else if(arg == "-type"){
            type = argv[++i];
            break;
        } 
    }

    //Διαβαζει από τα αρχεία ανάλογα με το type και αποθηκεύει στα vector
    vector<vector<double>> dataset;
    vector<vector<double>> queries;
    
    if(type == "mnist"){
        cout << "MNIST: " << endl;
        vector<vector<float>> float_data = return_mnist_data(input_file);
        vector<vector<float>> float_queries = return_mnist_queries(query_file);
        
        //μετατροπη data float->double ώστε να ειναι συμβατά με όλους τους αλγόριθμους
        dataset.resize(float_data.size());
        for(size_t i = 0; i < float_data.size(); i++){
            dataset[i].assign(float_data[i].begin(), float_data[i].end());
        }
        queries.resize(float_queries.size());
        for(size_t i = 0; i < float_queries.size(); i++){
            queries[i].assign(float_queries[i].begin(), float_queries[i].end());
        }  
    }
    else if(type == "sift"){
        cout << "SIFT: " << endl;
        vector<vector<float>> float_data = return_sift_data(input_file);
        vector<vector<float>> float_queries = return_sift_queries(query_file);
        
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

    // Έλεγχος οτι τα data φορτωθηκαν σωστα
    if(!dataset.empty()){
        cout << "Data size: " << dataset.size() << " vectors x " << dataset[0].size() << " dimensions" << endl;
    }

    if(use_lsh){
        cout << "\n>>> Running LSH Algorithm...\n";

        // Default τιμές για παραμέτρους
        int k = 10, L = 30, N = 5, seed = 1;
        double w = 1.5, R;
        bool range = false, R_flag = false;

        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];

            if (arg == "-o") output_file = argv[++i];
            else if (arg == "-k") k = stoi(argv[++i]);
            else if (arg == "-L") L = stoi(argv[++i]);
            else if (arg == "-w") w = stod(argv[++i]);
            else if (arg == "-N") N = stoi(argv[++i]);
            else if(arg == "-R"){ R_flag = true; R = stod(argv[++i]);}
            else if (arg == "-range") {
                string s = argv[++i];
                range = (s == "true");
            }
        }
        
        // Dataset-specific default τιμές για R
        if(!R_flag){
            if(type == "mnist") R = 2000.0;
            else if(type == "sift") R = 2.0;
        }

        lsh LSH(seed, input_file, query_file, output_file, k, L, w, N, R, type, range);
        LSH.print_params();
        LSH.lsh_func(dataset, queries);
    }
    else if(use_hypercube){
        cout << "\n>>> Running Hypercube Algorithm...\n";

        int kproj = 14, M = 10, N = 1, probes = 2, seed = 1;
        double w = 4.0, R;
        bool range = false, R_flag = false;

        for(int i = 1; i<argc; i++){
            string arg = argv[i];
            if(arg == "-kproj") kproj = stoi(argv[++i]);
            else if(arg == "-w") w = stod(argv[++i]);
            else if(arg == "-M") M = stoi(argv[++i]);
            else if(arg == "-probes") probes = stoi(argv[++i]); 
            else if(arg == "-o") output_file = argv[++i];
            else if(arg == "-N") N = stoi(argv[++i]);
            else if(arg == "-R"){ R_flag = true; R = stod(argv[++i]);}
            else if (arg == "-range") {
                string s = argv[++i];
                range = (s == "true");
            }
        }

        if(!R_flag){
            if(type == "mnist") R = 2000.0;
            else if(type == "sift") R = 2.0;
        }

        Hypercube hypercube(seed, input_file, query_file, output_file, kproj, M, w, N, R, probes, type, range);
        hypercube.print_params();
        hypercube.build_index(dataset);
        hypercube.hypercube_func(dataset, queries);
    }
    else if(use_ivfflat){
        cout << "\n>>> Running IVFFlat Algorithm...\n";
        
        int kclusters = 14, N = 1, nprobe = 2, seed = 1;
        double R;
        bool range = false, R_flag = false;

        for(int i = 1; i<argc; i++){
            string arg = argv[i];
            if(arg == "-kclusters") kclusters = stoi(argv[++i]);
            else if(arg == "-nprobe") nprobe = stoi(argv[++i]); 
            else if(arg == "-o") output_file = argv[++i];
            else if(arg == "-N") N = stoi(argv[++i]);
            else if(arg == "-R"){ R_flag = true; R = stod(argv[++i]);}
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

        IVFFlat ivfflat(seed, input_file, query_file, output_file, kclusters, nprobe, N, R, type, range);
        ivfflat.print_params();
        ivfflat.ivfflat_func(dataset, queries);
    }
    else if(use_ivfpq){
        cout << "\n>>> Running IVFPQ Algorithm... \n";

        int kclusters=50, nprobe=5, M=16, N=1, seed=1, nbits=8;
        double R;
        bool range, R_flag;

        for(int i=1; i<argc; i++){
            string arg = argv[i];
            if(arg == "-kclusters") kclusters = stoi(argv[++i]);
            else if(arg == "-nprobe") nprobe = stoi(argv[++i]);
            else if(arg == "-M") M = stoi(argv[++i]);
            else if(arg == "-o") output_file = argv[++i];
            else if(arg == "-N") N = stoi(argv[++i]);
            else if(arg == "-R"){ R_flag = true; R = stod(argv[++i]);}
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

        IVFPQ ivfpq(input_file, query_file, output_file, type, kclusters, nprobe, M, N, seed, nbits, R, range);
        ivfpq.print_params();
        ivfpq.ivfpq_func(dataset, queries);
    }
    else {
        cerr << "Error: You must specify an algorithm flag (-lsh, -hypercube, or -ivfflat)\n";
    }

    return 0;
}
