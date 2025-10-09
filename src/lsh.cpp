//LSH ALGORITHM
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <random>
#include <climits>
#include <cstdio>
#include <cstdlib>
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

    dimension = (type == "mnist") ? 784 : 128;

    cout << "LSH Constructed!" << endl;
}

lsh :: ~lsh(){
    cout << "LSH Deconstructed!" << endl;
}


//Αρχικοποίηση τυχαίων διανυσμάτων (v) κ μετατοπίσεων (t)
void lsh:: Initialize(){
    cout<<"Initialize v and t"<<endl;

    default_random_engine generator(seed);
    normal_distribution<double> normal_dist(0.0,1.0);
    uniform_real_distribution<double> uniform_dist(0.0, w);

    randomvectors.resize(L);
    randomshifts.resize(L);

    for(int i=0; i<L ; i++){
        randomvectors[i].resize(k);
        randomshifts[i].resize(k);
        
        for(int j=0; j<k ; j++){
            randomvectors[i][j].resize(dimension);  // Δημιουργία τυχαίου διανύσματος v διάστασης 178(SIFT)
            for(int dim = 0 ; dim < dimension ; dim++){
                randomvectors[i][j][dim] = normal_dist(generator);
            }
            randomshifts[i][j] = uniform_dist(generator);  // Δημιουργία τυχαίου διανύσματος t διάστασης 178(SIFT)
        }
    }

}

uint64_t lsh:: CreateHFun(const vector<double>& point, int tableIndex){
    int sum =0;
    
    for (int i=0; i<k; i++){
        double dot_product =0;
        for(int dim = 0 ; dim < dimension ; dim++){
            dot_product += randomvectors[tableIndex][i][dim]*point[i];
        } 

        if(dot_product > INT_MAX || dot_product < INT_MIN){
            cout<<"Overflow"<<endl;
            exit(EXIT_FAILURE);
        }
        double numerator = dot_product + randomshifts[tableIndex][i];
     
        cout<< w <<endl;
        return sum = floor( (double) (numerator) / w);
    }
}

//Δημιουργία hash tables
void lsh:: CreateHashTables(){
    cout<<"Create L:"<< L << "HashTables" <<endl;

    hashTables.resize(L);

    // for(int i =0 ; i < L; i++){
    //     for(int j =0 ; j < tzset.size(); j++){
    //         uint64_t hashV = CreateHFun(dataset[j],i);
    //         hashTables[i][j].push_back(j);
    //     }    
    // }
}

void lsh_func(){

    cout<<"LSH Algorithm" <<endl;

    //Initialize();
    //CreateHashTables();
    
}

void lsh:: print_params() {
    std::cout << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "k: " << k << ", L: " << L << ", w: " << w << ", N: " << N << ", R: " << R << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}
