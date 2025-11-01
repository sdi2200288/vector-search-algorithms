/* IVFFlat.hpp */

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
        int dimension;
        
        vector<vector<double>> centroids;           //k centroids
        vector<vector<int>> cluster_members;        //k-lists με indices μελών κάθε cluster
        vector<vector<double>> dataset_reference;   //αποθήκευση dataset
        
        double euclidean_distance(const vector<double>& v1, const vector<double>& v2);   //υπολογίζει την Ευκλείδεια απόσταση μεταξύ δύο διανυσμάτων
        vector<int> find_nearest_centroids(const vector<double>& p, int nprobe_count);   //εύρεση nprobe πλησιέστερων κεντροειδών
   
    public:
        IVFFlat(int seed1, string input_file1, string query_file1, string output_file1, int kclusters1, int nprobe1, int N1, double R1, string type1, bool range1);
        ~IVFFlat();
        void print_params();

        void Initialize(size_t dataset_n);
        void ivfflat_func(const vector<vector<double>>& dataset, const vector<vector<double>>& queries);
        void CreateClusters(const vector<vector<double>>& dataset);
        int find_nearest_centroid(const vector<double>& p);
        
        void Queries(const vector<vector<double>>& dataset,const vector<vector<double>>& queries);
        vector<pair<int, double>> ENN(const vector<vector<double>>& dataset,const vector<double>& query);  
        vector<pair<int, double>> ANN(const vector<vector<double>>& dataset,const vector<double>& query);
        vector<int> ARange_Search(const vector<vector<double>>& dataset, const vector<double>& query);
        vector<int> ERange_Search(const vector<vector<double>>& dataset, const vector<double>& query);
        double calculate_silhouette();
};

#endif