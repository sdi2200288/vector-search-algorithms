//LSH ALGORITHM

#include <iostream>
using namespace std;
#include "lsh.hpp"

lsh :: lsh(int seed1, std::string input_file1, std::string query_file1, std::string output_file1, int k1, int L1, float w1, int N1, float R1, std::string type1, bool range1){
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
}

lsh :: ~lsh(){
    cout << "Deconstructed!" << endl;
 }


