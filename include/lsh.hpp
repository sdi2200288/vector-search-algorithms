/* lsh.hpp */

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
#include <chrono>  
#include <algorithm> 
#include <fstream> 
#include <unordered_set>

using namespace std;

class lsh{
    private:
        string input_file, query_file, output_file, type;
        int k, L, N, seed;
        double w, R;
        bool range;
        int dimension; 
        size_t dataset_n;

        uint64_t M = 4294967291;            //prime ~ 2^32-5 για modulo operations
        vector<vector<uint64_t>> random_r;  //τυχαίοι συντελεστές L x k για hash functions
        size_t tableSize = 0;
        
        vector<unordered_map<int, vector<int>>> hashTables; //L hash-tables [bucket_id -> λίστα point indices]
        vector<vector<vector<double>>> randomvectors;       //τυχαία διανύσματα [L][k][dimension] για εσωτερικό γινόμενο
        vector<vector<double>> randomshifts;                //τυχαίες μετατοπίσεις [L][k] για hash functions

    public:
        lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, double w1, int N1, double R1, string type1, bool range1);
        ~lsh();
   
        void lsh_func(const vector<vector<double>>& dataset,const vector<vector<double>>& queries);

        void Initialize(size_t dataset_n);
        void CreateHashTables(const vector<vector<double>>& dataset);

        int CreateHFun(const vector<double>& point, int tableIndex);
        void print_params();
        void Queries(const vector<vector<double>>& queries,const vector<vector<double>>& dataset);
        double euclidean_distance(const vector<double>& v1, const vector<double>& v2);
        vector<pair<int, double>> ANN(const vector<double>& query,const vector<vector<double>>& dataset);
        vector<pair<int, double>> ENN(const vector<double>& query, const vector<vector<double>>& dataset);
        vector<int> ERange_Search(const vector<double>& query, const vector<vector<double>>& dataset);
        vector<int> ARange_Search(const vector<double>& query, const vector<vector<double>>& dataset);
    };

#endif