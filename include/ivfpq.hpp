/* ivfpq.hpp */

#ifndef IVFPQ_HPP
#define IVFPQ_HPP

#include <iostream>
#include <vector>
#include <utility>     

#include "ivfpq_index.hpp"

using namespace std;

class IVFPQ{
    private:
        string input_file, query_file, output_file, type;
        int kclusters, nprobe, M, N, seed, nbits;
        double R;
        bool range;

        ivfpq_index* index;                         //δείκτης στο ivfpq_index
        vector<vector<double>> dataset_reference;   //αποθήκευση αρχικού dataset

        double euclidean_distance(const vector<double>& v1, const vector<double>& v2);   //υπολογισμός Ευκλείδειας απόστασης

    public:
        IVFPQ(string input_file1, string query_file1, string output_file1, string type1,
              int kclusters1, int nprobe1, int M1, int N1, int seed1, int nbits1, 
              double R1, bool range1);
        ~IVFPQ();
        void print_params();

        vector<vector<double>> get_dataset_reference() const {return dataset_reference;}
        ivfpq_index* get_ivfpq_index() const {return index;}

        void build_index(const vector<vector<double>>& dataset);
        void Queries(const vector<vector<double>>& query, const vector<vector<double>>& dataset);
        void ivfpq_func(const vector<vector<double>>& dataset, const vector<vector<double>>& query);

        vector<pair<int, double>> approximate_nearest_neighbors(const vector<double>& query);
        vector<pair<int, double>> exact_nearest_neighbors(const vector<double>& query);
        vector<int> approximate_range_search(const vector<double>& query);
        vector<int> exact_range_search(const vector<double>& query, const vector<vector<double>>& dataset);

};

#endif