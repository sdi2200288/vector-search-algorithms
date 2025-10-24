#ifndef KMEANS_HPP
#define KMEANS_HPP

#include <iostream>
#include <vector>

using namespace std;

class k_means{
    private:
        int k;  //πληθος cluster
        int max_iterations;   //max πληθος επαναληψεων 
        double tolerance;   //ανοχη συγκλισης 
        vector<vector<double>> centroids;  //τρεχοντα κεντροειδη
        int seed;

        double euclidean_distance(const vector<double>& a, const vector<double>& b) const;
        void init_random_centroids(const vector<vector<double>>& data);
        int nearest_centroid(const vector<double>& point) const;
    public:
        k_means(int k, int max_iterations, double tolerance, int seed);

        vector<int> predict_fit(const vector<vector<double>>& data);
        vector<vector<double>> fit(const vector<vector<double>>& data);

        vector<vector<double>> get_centroids() const {return centroids;}
        vector<int> get_labels(const vector<vector<double>>& data) const;

        double calc_WCSS(const vector<vector<double>>& data, const vector<int>& labels) const;
        double calc_silhouette(const vector<vector<double>>& data, const vector<int>& get_labels) const;
};

#endif