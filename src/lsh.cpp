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


//Αρχικοποίηση τυχαίων διανυσμάτων (v) κ μετατοπίσεων (t)
// void lsh:: Initialize(){
//     cout<<"Initialize v and t"<<endl;

//     default_random_engine gen(seed);
//     normal_distribution<> normal(0.0,1.0);
//     uniform_real_distribution<> uniform(0.0, w);

//     vector<HashTablesParams>hashTables(L);

//     for(int i=0; i<L ; i++){
//         hashTables[i].v.resize(k*dimension);
//         hashTables[i].t.resize(k);
        
//         // Τυχαία αρχικοποίηση v και t
//         for(int j=0; j<k ; j++){
//             for(int d=0;d<dimension;d++){
//                   hashTables[i].v[j*dimension+d] = normal(gen);  // Δημιουργία τυχαίου διανύσματος v διάστασης 178(SIFT)
//             }
//         hashTables[i].t[j]= uniform(gen);  // Δημιουργία τυχαίου διανύσματος t διάστασης 178(SIFT)
//         }
//     }
// }

// uint64_t lsh:: CreateHFun(const vector<float> point,int tableIndex){
//     int sum=0;
// 	double dot_product=0.0;

//     for(int j=0; j<k;j++){
//         dot_product = 0.0;
//         for(int dim = 0; dim < dimension ; dim++){
//             dot_product += randomvectors[tableIndex][j][dim]*point[dim];
//         }
//         double t = randomshifts[tableIndex][j];
//         sum += floor((dot_product + t) / w);
//     }
 
// 	return static_cast<uint64_t>(sum);
// }

//Δημιουργία hash tables
// void lsh:: CreateHashTables(){
//     cout<<"Create L:"<< L << "HashTables" <<endl;


//     for(int idx =0 ; idx < (int)dataset.size(); idx++){
//         const auto &p = dataset[idx];

//         for(int l =0 ; l < L; l++){
//             vector<int> h_values(k);
            
//             for(int i = 0; i <L ; i++){
//                 double dot_p = 0.0;

//                 for(int d =0 ; d<dimension ; d++){
//                     dot_p += p[d]*hashTables[l].v[i * dimension + d];
//                 }

//                 h_values[i] = floor( (dot_p + hashTables[l].t[i] ) /w);
//             }
//              // Συνδυασμός h_values σε ένα ενιαίο hash key (super-hash)
//             size_t g_key = 0;
//             for (int i = 0; i < k; ++i) {
//                 g_key ^= std::hash<int>{}(h_values[i] + 0x9e3779b9 + (g_key << 6) + (g_key >> 2));
//             }

//             hashTables[l].table[g_key].push_back(idx);
//         }    
//     }
// }

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
    std::cout << "\n=== LSH PARAMETERS ===\n"
              << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "k: " << k << ", L: " << L << ", w: " << w << ", N: " << N << ", R: " << R << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}
