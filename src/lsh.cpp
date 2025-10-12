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
//#include "../include/sift_data.hpp"
//#include "../include/mnist_data.hpp"

using namespace std;

lsh :: lsh(int seed1, string input_file1, string query_file1, string output_file1, int k1, int L1, double w1, int N1, double R1, string type1, bool range1){
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

void lsh:: Initialize(size_t dataset_n){
    cout<<"Initialize v and t"<<endl;

    default_random_engine generator(seed);
    normal_distribution<double> normal_dist(0.0,1.0);
    uniform_real_distribution<double> uniform_dist(0.0, w);
    uniform_int_distribution<uint64_t> uniform_r(1, M-1);

    randomvectors.resize(L);
    randomshifts.resize(L);
    random_r.resize(L);

    for(int i=0; i<L ; i++){
        randomvectors[i].resize(k);
        randomshifts[i].resize(k);
        random_r[i].resize(k);

        for(int j=0; j<k ; j++){
            randomvectors[i][j].resize(dimension);  // Δημιουργία τυχαίου διανύσματος v διάστασης 178(SIFT)
            for(int dim = 0 ; dim < dimension ; dim++){
                randomvectors[i][j][dim] = normal_dist(generator);
            }
            randomshifts[i][j] = uniform_dist(generator);  // Δημιουργία τυχαίου διανύσματος t διάστασης 178(SIFT)
            random_r[i][j] = uniform_r(generator);
        }
    }
    if(dataset_n > 0){
        tableSize = max<size_t>(1, dataset_n / 4); // heuristic όπως στο slide
    }
}


int lsh:: CreateHFun(const vector<double>& point, int tableIndex){
    //int sum =0;
    vector<int64_t> hvals(k);
     
    for (int i=0; i<k; i++){
        double dot_product = 0.0;
        
        // for(int dim = 0 ; dim < dimension ; dim++){
        //     dot_product += randomvectors[tableIndex][i][dim]*point[i];
        // }
        //cout << tableIndex << "  " << randomvectors.size() << endl;
        int c=0;
        for(int dim = 0 ; dim < dimension ; dim++){
            //cout<< randomvectors[tableIndex][i][dim] << "  " << point[dim] << endl;
            dot_product += randomvectors[tableIndex][i][dim]*point[dim];
            c++;
        }
        cout<<c<<endl;
        if(dot_product > INT_MAX || dot_product < INT_MIN){
            cout<<"Overflow"<<endl;
            exit(EXIT_FAILURE);
        }

        //cout << randomshifts[tableIndex][i] << "  " << dot_product  << endl;
        double numerator = dot_product + randomshifts[tableIndex][i];
        //cout << numerator << endl; 

        //sum = floor( numerator / w);
        //cout << sum <<endl;   
        int64_t h_i = static_cast<int64_t>(floor(numerator / w) );
        hvals[i] = h_i;
    }   
    uint64_t ID = 0;
    for (int i = 0; i < k; ++i) {
        // διαχειριστούμε το πρόσημο του h_i: μετατρέπουμε σε uint64 με modular mapping
        int64_t h = hvals[i];
        uint64_t hu;
        if (h >= 0) {
            hu = static_cast<uint64_t>(h) % M;
        } 
        else {
            // αρνητικό: κάνουμε modular σωστό μετασχηματισμό
            int64_t tmp = h % static_cast<int64_t>(M);
            if (tmp < 0) tmp += static_cast<int64_t>(M);
            hu = static_cast<uint64_t>(tmp);
        }
        // πολλαπλασιασμός και πρόσθεση modulo M
        ID = ( ID + ( (random_r[tableIndex][i] * hu) % M ) ) % M;
    }
   // Τελική συνάρτηση g(p) = ID mod TableSize
    if (tableSize == 0) {
        // αν δεν έχεις ορίσει tableSize, επέστρεψε ID (ή κάνε fallback)
        return static_cast<int>(ID % 2147483647ULL); // fallback
    }
    return static_cast<int>( ID % tableSize );      
    
}

//Δημιουργία hash tables
void lsh:: CreateHashTables(const vector<vector<double>>& dataset){

    cout<<"Create L:"<< L << "HashTables" <<endl;

    hashTables.resize(L);

    int c=0;
    for(int i =0 ; i < L; i++){
        for(size_t j =0 ; j < dataset.size(); j++){
            //cout<<"Enter"<<endl;
            int hashV = CreateHFun(dataset[j],i);
            hashTables[i][hashV].push_back(j);
            c++;
        }    
        cout<<c<<endl;
    }
    cout << "Hash tables created successfully" << endl;
}

void lsh::lsh_func(const vector<vector<double>>& dataset){

    cout<<"LSH Algorithm" <<endl;

    cout << "Dataset size: " << dataset.size() << " vectors" << endl;
    cout << "Vector dimension: " << dataset[0].size() << endl;

    Initialize(dataset.size());
    CreateHashTables(dataset);
    //CreateHFun({1,2}, 2);
    
}

void lsh:: print_params() {
    std::cout << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "k: " << k << ", L: " << L << ", w: " << w << ", N: " << N << ", R: " << R << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}