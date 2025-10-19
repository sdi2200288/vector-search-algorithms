/* IVFPQ */

#include "iostream"

#include "ivfpq.hpp"

using namespace std;

IVFPQ :: IVFPQ(string input_file1, string query_file1, string output_file1, string type1, int kclusters1, int nprobe1, int M1, int N1, int seed1, int nbits1, double R1, bool range1){
    input_file = input_file1;
    query_file = query_file1;
    output_file = output_file1;
    type = type1;
    kclusters = kclusters1; 
    nprobe = nprobe1;
    M = M1;
    N = N1;
    seed = seed1;
    nbits = nbits1;
    R = R1;
    range = range1;

    cout << "IVFPQ Constructed!" << endl;
}

IVFPQ :: ~IVFPQ(){
    cout << "IVFPQ Deconstructed!" << endl;
}

void IVFPQ :: print_params(){
    cout << "input_file: " << input_file << "\n" << "query_file: " << query_file << "\n" 
         << "output_file: " << output_file << "\n" << "type: " << type << "\n" 
         << "kclusters: " << kclusters << "\n" << "nprobe: " << nprobe << "\n" << "M: " << M
         << "\n" << "N: " << N << "\n" << "seed: " << seed << "\n" << "nbits: " << nbits 
         << "\n" << "R: " << R << "\n" << "range: " << boolalpha << range << "\n";
}