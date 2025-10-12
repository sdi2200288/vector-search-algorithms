#ifndef LSH_HPP
#define LSH_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <string>
#include <functional>
#include <cmath>
#include <cstdint>

using namespace std;

class lsh{
        string input_file, query_file, output_file, type;
        int k, L, N, seed;
        double w, R;
        bool range;
        int dimension; //128 ή 784
        size_t dataset_n;

        // προσθήκη ως μέλη της κλάσης lsh
        uint64_t M = 4294967291; // prime ~ 2^32-5
        vector<vector<uint64_t>> random_r; // L x k
        size_t tableSize = 0;
        
        vector<unordered_map<int, vector<int>>> hashTables; // L hash-tables
        vector<vector<vector<double>>> randomvectors; // [L][k][dimension]
        vector<vector<double>> randomshifts;  //Τυχαίες μετατοπίσεις [L][k]

        

    public:
        lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, double w1, int N1, double R1, string type1, bool range1);
        ~lsh();
        
        //void lsh_func(int seed, std::string input_file, std::string query_file, std::string output_file, int k, int L, float w, int N, float R, std::string type, bool range);
   
        void lsh_func(const vector<vector<double>>& dataset);

        void Initialize(size_t dataset_n);
        void CreateHashTables(const vector<vector<double>>& dataset);

        int CreateHFun(const vector<double>& point, int tableIndex);
        void print_params();
};

#endif