#ifndef LSH_HPP
#define LSH_HPP

#include <iostream>

using namespace std;

class Hypercube{
    private:
        string input_file, query_file, output_file, type;
        int kproj, M, probes, N, seed;
        double w, R;
        bool range;

    public:
        Hypercube(int seed1, string input_file1, string query_file1, string output_file1, int kproj1, int M1, double w1, int N1, double R1, int probes1, string type1, bool range1);
        ~Hypercube();
        void print_params();

};

#endif