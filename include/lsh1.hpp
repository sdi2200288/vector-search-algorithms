//lsh.hpp

#include <iostream>
#include <vector>
#include <unordered_map>
#include <random>
#include <string>
#include <cmath>

using namespace std;


class lsh{
    private:
        string input_file, query_file, output_file, type;
        int k, L, N, seed;
        double w, R;
        bool range;
        int dimension; //128 ή 784

        vector<vector<float>> dataset; // Δεδομένα βάσης (vectors)
        vector<vector<float>> queries;

        vector<unordered_map<int, vector<int>>> hash_tables;    // L πίνακες hash

        vector<vector<vector<float>>> v;  // L × k × dim
        vector<vector<float>> t;

    public:
        lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, float w1, int N1, float R1, std::string type1, bool range1);
        ~lsh();
       
        void Initialize();
        void lsh_func();
        void print_params();
        int hi(const vector<float>& p, const vector<float>& v_i, float t_i); // μία hash function
        int compute_g(const vector<float>& p, int l);
        
};