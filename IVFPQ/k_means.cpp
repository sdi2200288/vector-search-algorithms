/* k - means */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

#include "k_means.hpp"

using namespace std;

k_means :: k_means(int k1, int max_iterations1, double tolerance1, int seed1){
    k = k1;
    max_iterations = max_iterations1;
    tolerance = tolerance1;
    seed = seed1;

    // cout << "k-means constructed!" << endl;
}

double k_means :: euclidean_distance(const vector<double>& a, const vector<double>& b) const {
    if(a.size() != b.size()){
        throw invalid_argument("Vectors of different dimensions!");
    }

    double dist = 0.0;
    for(size_t i=0; i<a.size(); i++){
        double diff = a[i] - b[i];
        dist += diff * diff; 
    }

    return sqrt(dist);
}

void  k_means :: init_random_centroids(const vector<vector<double>>& data){
    centroids.clear();
    mt19937 gen(seed);
    
    vector<size_t> indices;
    for(int i=0; i<k; i++){
        size_t idx;
        int attempts=0;

        do{

            idx = gen() % data.size();
            attempts++;

        } while(find(indices.begin(), indices.end(), idx) != indices.end() && attempts<100);

        indices.push_back(idx);
        centroids.push_back(data[idx]);
    }
}

int k_means :: nearest_centroid(const std::vector<double>& point) const {
    int best_index = 0;
    double best_distance = numeric_limits<double>::max();
    
    for (int i=0; i<k; i++){
        double dist = euclidean_distance(point, centroids[i]);
        if(dist<best_distance){
            best_distance = dist;
            best_index = i;
        }
    }

    return best_index;
}

vector<int> k_means :: predict_fit(const vector<vector<double>>& data){
    if(data.empty()){
        return {};
    }

    size_t n = data.size();
    size_t dim = data[0].size();

    // 1. Αρχικοποίηση κεντροειδών
    init_random_centroids(data);

    vector<int> labels(n, -1);
    vector<int> prev_labels(n, -1);

    // EM iterations
    for(int i=0; i<max_iterations; i++){

        // 2. Expectation Step: Ανάθεση σημείων στους πλησιέστερους κεντροειδείς
        for(size_t j=0; j<n; j++){
            labels[j] = nearest_centroid(data[j]);
        }

        // Έλεγχος σύγκλισης (αν τα labels δεν άλλαξαν)
        bool converged = true;
        for(size_t j=0; j<n; j++){
            if(labels[j] != prev_labels[j]){
                converged = false;
                break;
            }
        }

        if(converged && i>0){
            // cout << "Convergence in iteration: " << i << endl;
            break;
        }
        prev_labels = labels;

        // 3. Maximization Step: Υπολογισμός νέων κεντροειδών
        vector<vector<double>> new_centr(k, vector<double>(dim, 0.0));
        vector<int> cluster_size(k, 0);

        // Άθροιση σημείων ανά cluster
        for(size_t j=0; j<n; j++){
            int cluster_idx = labels[j];
            for(size_t d=0; d<dim; d++){
                new_centr[cluster_idx][d] += data[j][d];
            }

            cluster_size[cluster_idx]++;
        }

        // Υπολογισμός μέσου όρου (κεντροειδούς)
        for(int j=0; j<k; j++){
            if(cluster_size[j] > 0){
                for(size_t d=0; d<dim; d++){
                    new_centr[j][d] /= cluster_size[j];
                }
            }
            else{
                // cout << "Empty cluster: " << j << ", reinitialization..." << endl;

                mt19937 gen(seed + i);
                uniform_int_distribution<size_t> dis(0, n-1);
                new_centr[j] = data[dis(gen)];
            }
        }

        // 4. Έλεγχος σύγκλισης βάσει απόστασης κεντροειδών
        double max_centr_shift = 0.0;
        for(int j=0; j<k; j++){
            double shift = euclidean_distance(centroids[j], new_centr[j]);
            max_centr_shift = max(max_centr_shift, shift);
        }

        centroids = new_centr;

        if(max_centr_shift<tolerance){
            // cout << "Convergence based on tolerance in iteration: " << i << endl;
            break;
        }

        // if(i == max_iterations-1){
        //     cout << "You have reached maximum number of repetitions: " << max_iterations << endl;
        // }
    }

    return labels;
}
        
vector<vector<double>> k_means :: fit(const vector<vector<double>>& data){
    predict_fit(data); 
    return centroids;
}

vector<int> k_means :: get_labels(const vector<vector<double>>& data) const{
    if(centroids.empty()){
        throw runtime_error("The model has not been trained yet!");
    }

    vector<int> labels(data.size());
    for(size_t i=0; i<data.size(); i++){
        labels[i] = nearest_centroid(data[i]);
    }

    return labels;
}


/* Υπολογισμός Within-Cluster Sum of Squares (WCSS) */
double k_means :: calc_WCSS(const vector<vector<double>>& data, const vector<int>& labels) const {
    double wcss = 0.0;

    for(size_t i=0; i<data.size(); i++){
        int clust_idx = labels[i];
        double dist = euclidean_distance(data[i], centroids[clust_idx]);
        wcss += dist*dist;
    }

    return wcss;
}
              
double k_means :: calc_silhouette(const vector<vector<double>>& data, const vector<int>& labels) const {
    size_t n = data.size();
    if(n<=1){
        return 0.0;
    }

    vector<double> silhouette_scores(n);

    for(size_t i=0; i<n; i++){

        // Υπολογισμός μέσης απόστασης στο ίδιο cluster (a_i)
        double a_i = 0.0;
        int c_same = 0;

        for(size_t j=0; j<n; j++){
            if(i!=j && labels[j] == labels[i]){
                a_i += euclidean_distance(data[i], data[j]);
                c_same++;
            }
        }

        a_i = (c_same > 0) ? a_i / c_same : 0.0;

        // Υπολογισμός ελάχιστης μέσης απόστασης από άλλα clusters (b_i)
        double b_i = numeric_limits<double>::max();

        for(int clust_idx=0; clust_idx<k; clust_idx++){
            if(clust_idx == labels[i]){
                continue;
            }

            double avg_dist_oth = 0.0;
            int c_oth = 0;

            for(size_t j=0; j<n; j++){
                if(labels[j] == clust_idx){
                    avg_dist_oth += euclidean_distance(data[i], data[j]);
                    c_oth++;
                }
            }

            if(c_oth>0){
                avg_dist_oth /= c_oth;
                b_i = min(b_i, avg_dist_oth);
            }
        }

        // Υπολογισμός silhouette score για το σημείο i
        if(a_i == 0 && b_i == 0){
            silhouette_scores[i] = 0.0;
        }
        else{
            silhouette_scores[i] = (b_i - a_i)/max(a_i, b_i);
        }
    }

    // Μέσος silhouette score
    double score = 0.0;
    for(double i : silhouette_scores){
        score += i;
    }

    return score/n;
}
