/* hypercube.hpp */

#ifndef HYPERCUBE_HPP
#define HYPERCUBE_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>      //βιβλιοθήκη για το pair

using namespace std;

class Hypercube{
    private:
        string input_file, query_file, output_file, type;
        int kproj, M, probes, N, seed;
        double w, R;
        bool range;

        vector<vector<double>> v;                   //LSH vector για hash funcs
        vector<double> t;                           //LSH bias τιμές για hash funcs
        unordered_map<string, vector<int>> h_map;   //dομή hypercube (vertex -> λίστα point indices)

        vector<vector<double>> dataset_reference;   //αποθήκευση dataset για αναφορά κατά τα queries

        double euclidean_distance(const vector<double>& v1, const vector<double>& v2);  //υπολογίζει την Ευκλείδεια απόσταση μεταξύ δύο διανυσμάτων

    public:
        Hypercube(int seed1, string input_file1, string query_file1, string output_file1, int kproj1, 
            int M1, double w1, int N1, double R1, int probes1, string type1, bool range1);  
        ~Hypercube();   
        void print_params();

        int hypercube_dimension(const vector<vector<double>>& dataset, int kproj);              
        
        const unordered_map<string, vector<int>>& get_h_map()const{return h_map;}
        const vector<vector<double>>& get_dataset_reference()const{return dataset_reference;}

        void initialize_LSH_func(const vector<vector<double>>& dataset);
        string project_to_hypercube(const vector<vector<double>>& dataset, const vector<double>& point);
        void build_index(const vector<vector<double>>& dataset);
        
        vector<string> vertices_at_hamming_dist(const string& vertex, int target_dist, int max_vertices);     

        vector<pair<int, double>> exact_nearest_neighbors(const vector<double>& query);
        vector<pair<int, double>> approximate_nearest_neighbors(const vector<double>& query);
        vector<int> approximate_range_search(const vector<double>& query);
        vector<int> exact_range_search(const vector<double>& query);

        void Queries(const vector<vector<double>>& queries,const vector<vector<double>>& dataset);
        void hypercube_func(const vector<vector<double>>& dataset,const vector<vector<double>>& queries);
};

#endif