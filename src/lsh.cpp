//LSH ALGORITHM
#include <iostream>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>
#include <string>

#include "../include/lsh.hpp"

using namespace std;
lsh :: lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, float w1, int N1, float R1, string type1, bool range1){
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

    // Ορισμός διάστασης βάσει τύπου
    if (type == "mnist") dimension = 784;
    else dimension = 128; // SIFT default

    cout << "LSH Constructed!" << endl;
}

lsh :: ~lsh(){
    cout << "LSH Deconstructed!" << endl;
}

void lsh:: Initialize(){
    cout<<"Initialize v and t"<<endl;
    
    int dim = dimension;

    mt19937 gen(seed);
    normal_distribution<float> normal(0.0f,1.0f);         // v_i από Ν(0,1)
    uniform_real_distribution<float> uniform(0.0f, (float)w);   // t_i από [0, w)
    
    v.assign(L,vector<vector<float>>(k,vector<float>(dim)));
    t.assign(L,vector<float>(k));

    hash_tables.assign(L, unordered_map<int, vector<int>>());

    for(int l=0; l < L ; l++){
        // Τυχαία αρχικοποίηση v και t
        for(int i = 0 ; i < k ; i++){
            for(int d = 0 ; d < dim ; d++){
                  v[l][i][d] = normal(gen);  // Δημιουργία τυχαίου διανύσματος v διάστασης 128(SIFT)
            }

        t[l][i]= uniform(gen);  // Δημιουργία τυχαίου διανύσματος t διάστασης 128(SIFT)
        }
    }
}   


int lsh:: hi(const vector<float>& p,const vector<float>& v_i,float t_i){
	double dot=0.0;

    for(size_t j=0; j < p.size() ; j++){
        dot += (double)p[j]*(double)v_i[j];
    }
    cout << dot << endl;
	return  static_cast<int>(floor((dot + t_i) /w));
}

int lsh:: compute_g(const vector<float>& p,int l){
    int g=0;
    for(int i=0 ; i< k ; i++){
        int h = hi(p,v[l][i],t[l][i]);
        g = g * 31 + h;
    }
    return g;
}

void lsh::lsh_func(){

    cout<<"LSH Algorithm" <<endl;

    cout<<"Create L:"<< L << "HashTables" <<endl;

    if(dataset.empty()){
        int dim = dimension;
        int num_points = 100;
        mt19937 gen(seed);
        uniform_real_distribution<float> dist(0.0f,1.0f);
        dataset.resize(num_points,vector<float>(dim));

        for(auto &p :dataset){
            for(float &x : p){
                x = dist(gen);
            }
        }
    }

    for(size_t idx = 0 ; idx < 2 ; idx++){ //dataset.size()
        const auto &p = dataset[idx];

        for(int l = 0 ; l < L ; l++){
            int g = compute_g(p,l);
            hash_tables[l][g].push_back(idx);
        }
    }
    cout<<"Sucessful Crate HashTables"<<endl;
}

void lsh:: print_params() {
    std::cout << "\n=== LSH PARAMETERS ===\n"
              << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "k: " << k << ", L: " << L << ", w: " << w << ", N: " << N << ", R: " << R << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}
