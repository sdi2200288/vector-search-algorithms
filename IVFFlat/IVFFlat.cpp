//#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include "IVFFlat.hpp"
#include <fstream>
#include <climits>
#include <chrono>

// using namespace std;

IVFFlat::IVFFlat(int seed1, string input_file1, string query_file1, string output_file1, int kclusters1, int nprobe1, int N1, double R1, string type1, bool range1){
    seed = seed1;
    input_file = input_file1;
    query_file = query_file1;
    output_file = output_file1;
    kclusters = kclusters1;
    // M = M1;
    // w = w1;
    N = N1;
    R = R1;
    nprobe = nprobe1;
    type = type1;
    range = range1;

    cout << "IVFFlat Constructed!" << endl;
}

IVFFlat :: ~IVFFlat(){
    centroids.clear();
    cluster_members.clear();
    cout << "IVFFlat Deconstructed!" << endl;
}

double IVFFlat::euclidean_distance(const vector<double>& v1, const vector<double>& v2) {
    double sum = 0.0;
    for (size_t i = 0; i < v1.size(); i++) {
        double diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void IVFFlat::Initialize(size_t dataset_n){
    cout << "Initialize IVFFlat clusters" << endl;
    
    // Αρχικοποίηση δομών
    centroids.resize(kclusters);
    cluster_members.resize(kclusters);
    
    cout << "IVFFlat components initialized for " << kclusters << " clusters" << endl;
}

void IVFFlat::ivfflat_func(const vector<vector<double>>& dataset, const vector<vector<double>>& queries){
    if (dataset.empty()) {
        cerr << "Error: Dataset is empty" << endl;
        return;
    }
    if (queries.empty()) {
        cerr << "Error: Query set is empty" << endl;
        return;
    }
    
    cout << "IVFFlat Algorithm" << endl;
    cout << "Dataset size: " << dataset.size() << " vectors" << endl;
    cout << "Vector dimension: " << dataset[0].size() << endl;

    Initialize(dataset.size());
    
}

void IVFFlat :: print_params() {
    std::cout << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "kproj: " << kproj << ", M: " << M << ", w: " << w << ", N: " << N << ", R: " << R << ", probes: " << probes << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}