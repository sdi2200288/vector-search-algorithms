#ifndef LSH_HPP
#define LSH_HPP

#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class Hypercube{
    private:
        string input_file, query_file, output_file, type;
        int kproj, M, probes, N, seed;
        double w, R;
        bool range;

        vector<vector<double>> v;                   //LSH vector
        vector<double> t;                           //LSH bias
        unordered_map<string, vector<int>> h_map;   //hypercube structure

        vector<vector<double>> dataset_reference;   //αποθήκευση dataset για queries

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
        

};

#endif

