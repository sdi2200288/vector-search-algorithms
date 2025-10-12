#include <unistd.h>
#include <iostream>
#include <cstring>

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

    // Flags για ποιον αλγόριθμο θα τρέξει
    bool use_lsh = false;
    bool use_hypercube = false;
    bool use_ivfflat = false;

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
    }

    string input_file, query_file, output_file, type;
    int kproj = 4, M = 5, N = 1, probes = 1;
    double w = 4.0, R = 2000.0;
    bool range = false;

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
            else if(arg == "-R") R = stod(argv[++i]);
            else if(arg == "-type") type = argv[++i];
            else if (arg == "-range") {
                string s = argv[++i];
                range = (s == "true");
            }
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

