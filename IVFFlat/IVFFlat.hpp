#ifndef IVFFLAT_HPP
#define IVFFLAT_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility> 

using namespace std;

class IVFFlat{
    private:
        string input_file, query_file, output_file, type;
        int kclusters, nprobe, N, seed;
        double R;
        bool range;

        
        vector<vector<double>> centroids;           // k centroids
        vector<vector<int>> cluster_members; 
        vector<vector<double>> dataset_reference;   //αποθήκευση dataset για queries

        
        double euclidean_distance(const vector<double>& v1, const vector<double>& v2);                       //υπολογίζει την Ευκλείδεια απόσταση μεταξύ δύο διανυσμάτων

    public:
        IVFFlat(int seed1, string input_file1, string query_file1, string output_file1, int kclusters1, int nprobe1, int N1, double R1, string type1, bool range1);
        ~IVFFlat();
        void print_params();

        void Initialize(size_t dataset_n);
        void ivfflat_func(const vector<vector<double>>& dataset, const vector<vector<double>>& queries);

     
};

#endif

