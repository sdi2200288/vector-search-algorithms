#ifndef LSH_HPP
#define LSH_HPP

#include <iostream>

using namespace std;

class Hypercube{
    private:
        string input_file, query_file, output_file, type;
        int kproj, M, probes, N;
        double w, R;
        bool range;

};

#endif