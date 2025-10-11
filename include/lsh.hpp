#ifndef LSH_HPP
#define LSH_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <string>
#include <functional>

using namespace std;

class lsh{
        string input_file, query_file, output_file, type;
        int k, L, N, seed;
        double w, R;
        bool range;
        int dimension; //128 ή 784
        // std::vector<std::map<uint64_t, std::vector<int>> hashTables;

        vector<unordered_map<uint64_t, vector<int>>> hashTables; // L hash-tables
        vector<vector<vector<double>>> randomvectors; // [L][k][dimension]
        vector<vector<double>> randomshifts;  //Τυχαίες μετατοπίσεις [L][k]

        

    public:
        lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, float w1, int N1, float R1, std::string type1, bool range1);
        ~lsh();
        void lsh_func(int seed, std::string input_file, std::string query_file, std::string output_file, int k, int L, float w, int N, float R, std::string type, bool range);
   
        void lsh_func();

        void Initialize();
        void CreateHashTables();

        uint64_t CreateHFun(const vector<double>& point, int tableIndex);
        void print_params();
};

#endif