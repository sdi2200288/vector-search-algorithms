#include <iostream>
#include <cstring>
#include "../include/lsh.hpp"
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
        lsh LSH(seed, input_file, query_file, output_file, k, L, w, N, R, type, range);
        LSH.print_params();
        LSH.lsh_func();
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
 //lsh.hpp

#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <string>
#include <functional>

using namespace std;

class lsh{
    private:
        std::string input_file, query_file, output_file, type;
        int k, L, N, seed;
        double w, R;
        bool range;
        double * v;
        double t;

        int dimension; //178 ή 784

        

    public:
        lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, float w1, int N1, float R1, std::string type1, bool range1);
        ~lsh();
       
        void lsh_func();

        void Initialize();
        void CreateHashTables();
        void print_params();
};

//LSH ALGORITHM
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include "../include/lsh.hpp"
#include "../include/mnist_data.hpp"
#include "../include/sift_data.hpp"


using namespace std;
lsh :: lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, float w1, int N1, float R1, string type1, bool range1){
    seed = seed1;
    input_file = input_file1;
    query_file = query_file1;
    output_file = output_file1;
    k = k1;
    L = L1;
    w = w1;
    N = N1;
    R = R1;
    type = type1;
    range = range1;

    dimension = (type == "mnist") ? 784 : 128;

    cout << "LSH Constructed!" << endl;
}

lsh :: ~lsh(){
    cout << "LSH Deconstructed!" << endl;
}
                                                               