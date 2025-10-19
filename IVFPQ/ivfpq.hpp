#ifndef IVFPQ_HPP
#define IVFPQ_HPP

#include <iostream>

using namespace std;

class IVFPQ{
    private:
        string input_file, query_file, output_file, type;
        int kclusters, nprobe, M, N, seed, nbits;
        double R;
        bool range;

    public:
        IVFPQ(string input_file1, string query_file1, string output_file1, string type1,
              int kclusters1, int nprobe1, int M1, int N1, int seed1, int nbits1, 
              double R1, bool range1);
        ~IVFPQ();
        void print_params();
};

#endif