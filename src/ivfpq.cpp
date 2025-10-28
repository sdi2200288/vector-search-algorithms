/* IVFPQ */

#include "iostream"
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <limits>
#include <chrono>

#include "../include/ivfpq.hpp"

using namespace std;

IVFPQ :: IVFPQ(string input_file1, string query_file1, string output_file1, string type1, int kclusters1, int nprobe1, int M1, int N1, int seed1, int nbits1, double R1, bool range1){
    input_file = input_file1;
    query_file = query_file1;
    output_file = output_file1;
    type = type1;
    kclusters = kclusters1; 
    nprobe = nprobe1;
    M = M1;
    N = N1;
    seed = seed1;
    nbits = nbits1;
    R = R1;
    range = range1;

    index = nullptr;

    cout << "IVFPQ Constructed!" << endl;
}

IVFPQ :: ~IVFPQ(){
    if(index != nullptr){
        delete index;
    }

    cout << "IVFPQ Deconstructed!" << endl;
}

void IVFPQ :: print_params(){
    cout << "input_file: " << input_file << "\n" << "query_file: " << query_file << "\n" 
         << "output_file: " << output_file << "\n" << "type: " << type << "\n" 
         << "kclusters: " << kclusters << "\n" << "nprobe: " << nprobe << "\n" << "M: " << M
         << "\n" << "N: " << N << "\n" << "seed: " << seed << "\n" << "nbits: " << nbits 
         << "\n" << "R: " << R << "\n" << "range: " << boolalpha << range << "\n";
}

double IVFPQ :: euclidean_distance(const vector<double>& v1, const vector<double>& v2){
    if(v1.size() != v2.size()){
        throw invalid_argument("Vectors of diffenrent dimensions!");
    }

    double dist = 0.0;
    for(size_t i=0; i<v1.size(); i++){
        double diff = v1[i] - v2[i];
        dist += diff*diff;
    }

    return sqrt(dist);
}

void IVFPQ :: build_index(const vector<vector<double>>& dataset){
    cout << "Building IVFPQ index..." << endl;
    
    if(dataset.empty()){
        throw invalid_argument("Cannot build index with empty dataset!");
    }

    dataset_reference = dataset;
    int dim = dataset[0].size();
    index = new ivfpq_index(kclusters, M, nbits, dim);
    index->build(dataset);

    cout << "IVFPQ Index built successfully!" << endl;
}

vector<pair<int, double>> IVFPQ :: approximate_nearest_neighbors(const vector<double>& query){
    if(index == nullptr){
        throw runtime_error("IVFPQ Index not built yet!");
    }

    auto ivfpq_res = index->Query(query, nprobe, N);
    vector<pair<int, double>> res;

    for(const auto& i : ivfpq_res){
        res.emplace_back(i.second, i.first);
    }

    return res;
}

vector<pair<int, double>> IVFPQ :: exact_nearest_neighbors(const vector<double>& query){
    vector<pair<double, int>> dists;

    for(size_t i=0; i<dataset_reference.size(); i++){
        double dist = euclidean_distance(query, dataset_reference[i]);
        dists.emplace_back(dist, i);
    }

    sort(dists.begin(), dists.end());

    vector<pair<int, double>> res;

    for(int i=0; i<min(N, static_cast<int>(dists.size())); i++){
        res.emplace_back(dists[i].second, dists[i].first);
    }

    return res;
}

/*vector<int> IVFPQ :: approximate_range_search(const vector<double>& query){
    if(index == nullptr){
        throw runtime_error("IVFPQ Index not built yet!");
    }

    auto ivfpq_res = index->range_search(query, nprobe, R);
    vector<int> res;

    for(const auto& i : ivfpq_res){
        res.push_back(i.second);
    }

    return res;
}*/

vector<int> IVFPQ :: approximate_range_search(const vector<double>& query){
    if(index == nullptr){
        throw runtime_error("IVFPQ Index not built yet!");
    }

    // Χρησιμοποίησε μεγαλύτερο R για την προσεγγιστική αναζήτηση
    double approx_R = R * 1.5;  // Πειραματική αύξηση 50%
    
    auto ivfpq_res = index->range_search(query, nprobe, approx_R);
    vector<int> res;

    for(const auto& candidate : ivfpq_res){
        res.push_back(candidate.second);
    }

    return res;
}

vector<int> IVFPQ :: exact_range_search(const vector<double>& query, const vector<vector<double>>& dataset){
    vector<int> res;

    for(size_t i=0; i<dataset.size(); i++){
        double dist = euclidean_distance(query, dataset[i]);
        if(dist<=R){
            res.push_back(i);
        } 
    }

    return res;
}

void IVFPQ :: Queries(const vector<vector<double>>& query, const vector<vector<double>>& dataset){
    cout << "Executing IVFPQ queries..." << endl;

    ofstream out(output_file);
    if(!out.is_open()){
        throw runtime_error("Cannot open output file: " + output_file);
    }

    out << "IVFPQ" << endl;

    double total_af = 0.0;
    double total_recall = 0.0;
    double total_approx_time = 0.0;
    double total_exact_time = 0.0;
    int success_queries = 0;

    auto start_all = chrono::high_resolution_clock::now();

    for(size_t i=0; i<query.size(); i++){
        const auto& q = query[i];
        
        // Approxximate search
        auto approx_start = chrono::high_resolution_clock::now();
        auto ann_res = approximate_nearest_neighbors(q);
        auto approx_range_res = range ? approximate_range_search(q) : vector<int>();
        auto approx_end = chrono::high_resolution_clock::now();
        double approx_time = chrono::duration<double>(approx_end-approx_start).count();

        // Exact search
        auto exact_start = chrono::high_resolution_clock::now();
        auto enn_res = exact_nearest_neighbors(q);
        auto exact_range_res = range ? exact_range_search(q, dataset) : vector<int>();
        auto exact_end = chrono::high_resolution_clock::now();
        double exact_time = chrono::duration<double>(exact_end-exact_start).count();

        //Εγγραφή Αποτελεσμάτων
        out << "Query: " << i << endl;
        
        //Nearest neigbors
        for(int j=0; j<N; j++){
            if(static_cast<size_t>(j)<ann_res.size()){
                out << "Nearest neighbor-" << (j+1) << ": " << ann_res[j].first << endl;
                out << "distanceApproximate: " << ann_res[j].second << endl;
                
                if(static_cast<size_t>(j)<enn_res.size()){
                    out << "distanceTrue: " << enn_res[j].second << endl;
                }
                else{
                    out << "distanceTrue: " << -1 << endl;
                }
            }
        }

        //Range search
        if(range){
            out << "R-near neighbors:" << endl;
            for(int point_id : approx_range_res){
                out << point_id << endl;
            }

            if(approx_range_res.empty()){
                out << "None" << endl;
            }
        }

        //Υπολογισμός μετρικών
        if(!ann_res.empty() && !enn_res.empty()){
            double approx_dist = ann_res[0].second;
            double exact_dist = enn_res[0].second;

            if(exact_dist>0){
                total_af += (approx_dist/exact_dist);
            }

            int correct_matches = 0;
            for(const auto& ann : ann_res){
                for(const auto& enn : enn_res){
                    if(ann.first == enn.first){
                        correct_matches++;
                        break;
                    }
                }
            }

            total_recall += static_cast<double>(correct_matches)/N;

            total_approx_time += approx_time;
            total_exact_time += exact_time;
            success_queries++;
        }
        out << "-------------------" << endl;

        // out << endl;
    
        // //Εγγραφή τελικών μετρικών
        // if(success_queries>0){
        //     double avg_af = total_af/success_queries;
        //     double avg_recall = total_recall/success_queries;
        //     double qps = success_queries/total_approx_time;
        //     double taa = total_approx_time/success_queries;
        //     double tra = total_exact_time/success_queries;

        //     out << "Average AF: " << avg_af << endl;
        //     out << "Recall@N: " << avg_recall << endl;
        //     out << "QPS: " << qps << endl;
        //     out << "tApproximateAverage: " << taa << endl;
        //     out << "tTrueAverage: " << tra << endl;
        // }
        // else{
        //     out << "Average AF: 0" << endl;
        //     out << "Recall@N: 0" << endl;
        //     out << "QPS: 0" << endl;
        //     out << "tApproximateAverage: 0" << endl;
        //     out << "tTrueAverage: 0" << endl;
        // }

        // out << endl;
    
    }
    
    auto end_all = chrono::high_resolution_clock::now();
    double total_time_all = chrono::duration<double>(end_all - start_all).count();
    
    //Εγγραφή τελικών μετρικών
    if(success_queries>0){
        double avg_af = total_af/success_queries;
        double avg_recall = total_recall/success_queries;
        double qps = query.size()/total_time_all;
        double taa = total_approx_time/success_queries;
        double tra = total_exact_time/success_queries;
    
        out << "FINAL METRICS:" << endl;
        out << "Total Average AF: " << avg_af << endl;
        out << "Total Average Recall@N: " << avg_recall << endl;
        out << "QPS: " << qps << endl;
        out << "Average Approximate Time: " << taa << endl;
        out << "Average Exact Time: " << tra << endl;
    }
    // else{
    //     out << "AF: 0" << endl;
    //     out << "Recall@N: 0" << endl;
    //     out << "QPS: 0" << endl;
    //     out << "tApproximateAverage: 0" << endl;
    //     out << "tTrueAverage: 0" << endl;
    // }

    out.close();
    cout << "Results written to: " << output_file << endl;
    cout << "Successful queries processed: " << success_queries << "/" << query.size() << endl;
}
    
void IVFPQ :: ivfpq_func(const vector<vector<double>>& dataset, const vector<vector<double>>& query){
    cout << "=== Starting IVFPQ Algorithm ===" << endl;

    build_index(dataset);
    Queries(query, dataset);

    cout << "=== IVFPQ Algorithm Completed ===";
}