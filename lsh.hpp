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

        vector<unordered_map<uint64_t, vector<int>>> hashTables; // L hash-tables
        vector<vector<vector<double>>> randomvectors; // [L][k][dimension]
        vector<vector<double>> randomshifts;  //Τυχαίες μετατοπίσεις [L][k]
       
        int dimension; //178 ή 784

        

    public:
        lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, float w1, int N1, float R1, std::string type1, bool range1);
        ~lsh();
       
        void lsh_func();

        void Initialize();
        void CreateHashTables();
        uint64_t CreateHFun(const vector<double>& point, int tableIndex);
        void print_params();
};

