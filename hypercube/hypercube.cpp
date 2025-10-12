//Hypercube.cpp

// #include <iostream>

#include "hypercube.hpp"

// using namespace std;

Hypercube :: Hypercube(int seed1, string input_file1, string query_file1, string output_file1, int kproj1, int M1, double w1, int N1, double R1, int probes1, string type1, bool range1){
    seed = seed1;
    input_file = input_file1;
    query_file = query_file1;
    output_file = output_file1;
    kproj = kproj1;
    M = M1;
    w = w1;
    N = N1;
    R = R1;
    probes = probes1;
    type = type1;
    range = range1;

    cout << "LSH Constructed!" << endl;
}

Hypercube :: ~Hypercube(){
    cout << "LSH Deconstructed!" << endl;
}

void Hypercube :: print_params() {
    std::cout << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "kproj: " << kproj << ", M: " << M << ", w: " << w << ", N: " << N << ", R: " << R << ", probes: " << probes << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}