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
    hashTables.clear();
    randomvectors.clear();
    randomshifts.clear();
    random_r.clear();
    cout << "LSH Deconstructed!" << endl;
}

//Αρχικοποίηση τυχαίων διανυσμάτων (v) κ μετατοπίσεων (t)
void lsh:: Initialize(size_t dataset_n){
    cout<<"Initialize v and t and r"<<endl;

    default_random_engine generator(seed);
    normal_distribution<double> normal_dist(0.0,1.0);
    uniform_real_distribution<double> uniform_dist(0.0, w);
    uniform_int_distribution<uint64_t> uniform_r(1, M-1);

    randomvectors.resize(L);
    randomshifts.resize(L);
    random_r.resize(L);

    for(int i = 0; i < L; i++){
        randomvectors[i].resize(k);
        randomshifts[i].resize(k);
        random_r[i].resize(k);

        for(int j = 0; j < k; j++){
            randomvectors[i][j].resize(dimension);
            for(int dim = 0; dim < dimension; dim++){
                randomvectors[i][j][dim] = normal_dist(generator);
            }
            randomshifts[i][j] = uniform_dist(generator);
            random_r[i][j] = uniform_r(generator);
        }
    }
    if(dataset_n > 0){
        tableSize = max<size_t>(1, dataset_n / 4);
    }
    cout << "Random components initialized successfully" << endl;
}


double lsh::euclidean_distance(const vector<double>& v1, const vector<double>& v2) {
    double sum = 0.0;
    for (size_t i = 0; i < v1.size(); i++) {
        double diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

vector<pair<int, double>> lsh::ANN(const vector<double>& query,const vector<vector<double>>& dataset){
    unordered_set<int> candidate_set;

    for(int i = 0; i < L; i++){
        int query_hash = CreateHFun(query,i);

        if(hashTables[i].find(query_hash) != hashTables[i].end()){

            auto it = hashTables[i].find(query_hash);
            if(it != hashTables[i].end()){
                candidate_set.insert(it->second.begin(), it->second.end());
            }
        }
    }

    vector<pair<int,double>> candidate_distances;
    for(int j : candidate_set){
        double dist = euclidean_distance(query,dataset[j]);
        candidate_distances.push_back({j,dist});
    }

    sort(candidate_distances.begin(),candidate_distances.end(),[](const auto& a, const auto& b) { return a.second < b.second; });

    if(candidate_distances.size()>N){
        candidate_distances.resize(N);
    }

    return candidate_distances;
}

vector<pair<int, double>> lsh::ENN(const vector<double>& query,const vector<vector<double>>& dataset){
    vector<pair<int,double>> all_distances;
    
    for(size_t i = 0; i < dataset.size(); i++){
        double dist = euclidean_distance(query,dataset[i]);
        all_distances.push_back({i,dist});
    }
        
    sort(all_distances.begin(),all_distances.end(),[](const auto& a, const auto& b) { return a.second < b.second; });

    if(all_distances.size() > N){
        all_distances.resize(N);
    }
    return all_distances;
}

void lsh::Queries(const vector<vector<double>>& queries,const vector<vector<double>>& dataset){
    cout<<"Calculate Queries"<<endl;

    ofstream out(output_file);
    if(!out.is_open()){
        cerr << "Error: Cannot open output file " << output_file << endl;
        return;
    }
    
    cout<<"LSH\n\n";

    double total_Average_factor = 0.0,total_recall=0.0;
    double total_atime = 0.0,total_etime = 0.0;
    int valid_q = 0;

    auto start_all = chrono::high_resolution_clock::now();
    
    for(int i = 0; i < queries.size(); i++){
        const auto& q = queries[i];

        //Προσεγγιστική αναζήτηση
        auto start = chrono::high_resolution_clock::now();
        auto aNN = ANN(q,dataset,N);    // vector<pair<int,double>>
        auto end = chrono::high_resolution_clock::now();
        auto atime = chrono::duration<double>(end-start).count();
        
        //Ακριβής αναζήτηση
        start = chrono::high_resolution_clock::now();
        auto eNN = ENN(q,dataset,N);    //vector<pair<int,double>>
        end = chrono::high_resolution_clock::now();
        auto etime = chrono::duration<double>(end-start).count();
        
        valid_q++;
        total_atime += atime;
        total_etime += etime;
   
        out << "Query: " << i <<endl;
        if(!aNN.empty() && !eNN.empty()) {
            
            for(int neighbor = 0; neighbor < aNN.size(); neighbor++){
                int data_idx =  aNN[neighbor].first;
                
                out << "Nearest neighbor-" << neighbor + 1 << ": " << data_idx << endl ;
                out << "distanceApproximate: " << aNN[neighbor].second << endl;
        
                if(neighbor < eNN.size()){
                    out<<"distanceTrue: " << eNN[neighbor].second << endl;
                }
            }
            
            double Average_factor = aNN[0].second / eNN[0].second;
            total_Average_factor += Average_factor;
            
            int recall_hits = 0;
            for(const auto& exact : eNN){
                for(const auto& approx : aNN){
                    if(exact.first == approx.first){
                        recall_hits++;
                        break;
                    }
                }
            }
            
            double recall = (double)recall_hits / (double)eNN.size();
            total_recall += recall;
            
            //Range Search
            if(range){
                vector<int> range_neighbors;

                for(size_t j = 0; j < dataset.size(); j++){
                    double dist = euclidean_distance(q,dataset[j]);
                    if(dist <= R){
                        range_neighbors.push_back(j);
                    }
                }

                out << "R-near neighbors:";
                if(range_neighbors.empty()){
                    out << "\n";
                }else{
                    out<< "\n";
                    for(int id: range_neighbors){
                        out << id << "\n";
                    }
                }
            }

            double total_time_all = chrono::duration<double>(chrono::high_resolution_clock::now()-start_all).count();
            double qps = (valid_q > 0) ? valid_q / total_time_all : 0.0;

            out<< "Average AF: " << Average_factor << endl;
            out<< "Recall@N: " << recall <<endl;
            out << "QPS: " << qps << endl;
            out << "tApproximateAverage: " << atime << endl;
            out << "tTrueAverage: " << etime << endl;
            out << "-------------------" << endl;
        }     
    }

    out.close();
    cout<<"Complete Queries"<<endl;

   
}

int lsh:: CreateHFun(const vector<double>& point, int tableIndex){
    vector<int64_t> hash_values(k);
     
    for (int i = 0; i < k; i++){
        double dot_product = 0.0;
                
        for(int dim = 0 ; dim < dimension ; dim++){
            dot_product += randomvectors[tableIndex][i][dim]*point[dim];
        }
        
        if(dot_product > INT_MAX || dot_product < INT_MIN){
            cout<<"Overflow"<<endl;
            exit(EXIT_FAILURE);
        }

        double numerator = dot_product + randomshifts[tableIndex][i];
        int64_t h_i = static_cast<int64_t>(std::floor(numerator / w) );
        hash_values[i] = h_i;
    } 

    uint64_t ID = 0;
    for (int i = 0; i < k; ++i) {
        int64_t h = hash_values[i];
        uint64_t hu = static_cast<uint64_t>((h % static_cast<int64_t>(M) + static_cast<int64_t>(M)) % static_cast<int64_t>(M));
        ID = ( ID +  (random_r[tableIndex][i] * hu) % M ) % M;
    }

    // Τελική συνάρτηση g(p) = ID mod TableSize
    if (tableSize == 0) {
        // αν δεν έχεις ορίσει tableSize
        return static_cast<int>(ID % 2147483647ULL);
    }

    return static_cast<int>( ID % tableSize );      
}

//Δημιουργία hash tables
void lsh:: CreateHashTables(const vector<vector<double>>& dataset){

    if(dataset.empty()) {
        cerr << "Error: Empty dataset" << endl;
        return;
    }

    cout<<"Create L: "<< L << " hash tables" <<endl;
    hashTables.resize(L);
    
    for(int i = 0; i < L; i++){
        for(size_t j = 0; j < dataset.size(); j++){
            if(dataset[j].size() != dimension) {
                cerr << "Error: Vector dimension mismatch" << endl;
                continue;
            }
            int hash_value = CreateHFun(dataset[j],i);
            hashTables[i][hash_value].push_back(j);
        }       
    }
    cout << "Hash tables created successfully" << endl;
}

void lsh::lsh_func(const vector<vector<double>>& dataset,const vector<vector<double>>& queries){

    cout<<"LSH Algorithm" <<endl;

    cout << "Dataset size: " << dataset.size() << " vectors" << endl;
    cout << "Vector dimension: " << dataset[0].size() << endl;

    Initialize(dataset.size());
    CreateHashTables(dataset);
    Queries(queries,dataset);
}

void lsh:: print_params() {
    std::cout << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "k: " << k << ", L: " << L << ", w: " << w << ", N: " << N << ", R: " << R << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}