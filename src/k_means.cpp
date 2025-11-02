/* k - means */

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>

#include "../include/k_means.hpp"

using namespace std;

//constructor
k_means :: k_means(int k1, int max_iterations1, double tolerance1, int seed1){
    k = k1;
    max_iterations = max_iterations1;
    tolerance = tolerance1;
    seed = seed1;
}

//συνάρτηση που υπολογίζει την ευκλείδεια απόσταση 
double k_means :: euclidean_distance(const vector<double>& a, const vector<double>& b) const {
    if(a.size() != b.size())
        throw invalid_argument("Vectors of different dimensions!");

    double dist = 0.0;
    for(size_t i=0; i<a.size(); i++){
        double diff = a[i] - b[i];
        dist += diff * diff; 
    }

    return sqrt(dist);
}

///συνάρτηση που υπολογίζει τη τετραγωνική ευκλείδεια απόσταση (για βελτιστοποίηση)
double k_means :: squared_euclidean_distance(const vector<double>& a, const vector<double>& b) const {
    if(a.size() != b.size())
        throw invalid_argument("Vectors of different dimensions!");

    double dist = 0.0;
    for(size_t i=0; i<a.size(); i++){
        double diff = a[i] - b[i];
        dist += diff * diff; 
    }

    return dist; //χωρίς sqrt (για ταχύτερους υπολογισμούς)
}

//συνάρτηση που αρχικοποεί τα centroids με τυχαία επιλογή σημείων
void k_means :: init_random_centroids(const vector<vector<double>>& data){
    centroids.clear();
    mt19937 gen(seed);  //mersenne Twister generator
    
    vector<size_t> indices; //για αποφυγή διπλότυπων centroids
    for(int i=0; i<k; i++){
        size_t idx;
        int attempts=0;

        do{     //επιλογή μοναδικού τυχαίου σημείου
            idx = gen() % data.size();  //τυχαίο index
            attempts++;
        } while(find(indices.begin(), indices.end(), idx) != indices.end() && attempts<100);

        indices.push_back(idx);
        centroids.push_back(data[idx]); //προσθήκη σημείου ως centroid
    }
}

//συνάρτηση που κάνει εύρεση του πλησιέστερου centroid σε ένα σημείο
int k_means :: nearest_centroid(const std::vector<double>& point) const {
    int best_index = 0;
    double best_distance = numeric_limits<double>::max();
    
    for (int i=0; i<k; i++){    //έλεγχος απόστασης από όλα τα centroids
        double dist = squared_euclidean_distance(point, centroids[i]);
        if(dist < best_distance){
            best_distance = dist;
            best_index = i;
        }
    }

    return best_index;
}

//συνάρτηση που κάνει αρχικοποίηση centroids με k-means++ algorithm
void k_means :: init_kmeans_plusplus(const vector<vector<double>>& data){
    centroids.clear();
    mt19937 gen(seed);
    
    //1ο centroid τυχαίο
    uniform_int_distribution<size_t> dis(0, data.size()-1);
    centroids.push_back(data[dis(gen)]);
    
    for(int i = 1; i < k; i++){     //επιλογή των υπόλοιπων k-1 centroids
        vector<double> distances(data.size());
        double total_distance = 0.0;
        
        for(size_t j = 0; j < data.size(); j++){    //υπολογισμός αποστάσεων προς το πλησιέστερο centroid
            double min_dist = numeric_limits<double>::max();
            for(const auto& centroid : centroids){
                double dist = squared_euclidean_distance(data[j], centroid);
                min_dist = min(min_dist, dist);
            }
            distances[j] = min_dist; 
            total_distance += distances[j];
        }
        
        uniform_real_distribution<double> prob_dis(0.0, total_distance);    //επέλεξε επόμενο centroid με πιθανότητα ανάλογη της απόστασης
        double r = prob_dis(gen);
        double cumulative = 0.0;
        
        for(size_t j = 0; j < data.size(); j++){
            cumulative += distances[j];
            if(cumulative >= r){
                centroids.push_back(data[j]);
                break;
            }
        }
    }
}

//συνάρτηση που υλοποιεί τη κύρια μέθοδο εκπαίδευσης k-means 
vector<int> k_means :: predict_fit(const vector<vector<double>>& data){
    if(data.empty())
        return {};

    size_t n = data.size();
    size_t dim = data[0].size();

    //1. Χρησιμοποίησε k-means++ για καλύτερη αρχικοποίηση
    init_kmeans_plusplus(data);

    //αρχικοποίηση μεταβλητών
    vector<int> labels(n, -1);                                  //labels για κάθε σημείο
    vector<vector<double>> sums(k, vector<double>(dim, 0.0));   //αθροίσματα ανά cluster
    vector<int> counts(k, 0);                                   //πλήθος σημείων ανά cluster

    for(int iter = 0; iter < max_iterations; iter++){
        bool changed = false;

        //2. Ενημέρωση labels - αντιστοίχιση σημείων στους πλησιέστερους centroids
        for(size_t i = 0; i < n; i++){
            int best_cluster = nearest_centroid(data[i]);
            if(labels[i] != best_cluster){
                labels[i] = best_cluster;
                changed = true;     //σημείο άλλαξε cluster
            }
        }

        if(!changed && iter > 0) break;     //έλεγχος σύγκλισης αν δεν άλλαξε κανένα label

        //3. Επαναφορά αθροισμάτων και counts
        fill(sums.begin(), sums.end(), vector<double>(dim, 0.0));
        fill(counts.begin(), counts.end(), 0);

        //4. Άθροιση συντεταγμένων για κάθε cluster
        for(size_t i = 0; i < n; i++){
            int cluster = labels[i];
            for(size_t d = 0; d < dim; d++)
                sums[cluster][d] += data[i][d]; //άθροισμα συντεταγμένων
            
            counts[cluster]++;                  //αύξηση μετρητή
        }

        //5. Υπολογισμός νέων κεντροειδών
        for(int j = 0; j < k; j++){
            if(counts[j] > 0){
                for(size_t d = 0; d < dim; d++)
                    centroids[j][d] = sums[j][d] / counts[j];   //μέσος όρος
            }
            else{
                //επαναρχικοποίηση κενού cluster
                mt19937 gen(seed + iter);
                uniform_int_distribution<size_t> dis(0, n-1);
                centroids[j] = data[dis(gen)];  //τυχαίο σημείο
            }
        }
    }

    return labels;
}
  
//συνάρτηση που κάνει εκπαίδευση k-means και επιστρέφει τα centroids
vector<vector<double>> k_means :: fit(const vector<vector<double>>& data){
    predict_fit(data); 
    return centroids;
}

//συνάρτηση που κάνει πρόβλεψη labels για νέα δεδομένα
vector<int> k_means :: get_labels(const vector<vector<double>>& data) const{
    if(centroids.empty())
        throw runtime_error("The model has not been trained yet!");

    vector<int> labels(data.size());
    for(size_t i=0; i<data.size(); i++)
        labels[i] = nearest_centroid(data[i]);  //αντιστοίχιση στο πλησιέστερο centroid

    return labels;
}

//συνάρτηση που κάνει υπολογισμό Within-Cluster Sum of Squares (WCSS)
double k_means :: calc_WCSS(const vector<vector<double>>& data, const vector<int>& labels) const {
    double wcss = 0.0;

    for(size_t i=0; i<data.size(); i++){    //άθροισμα τετραγώνων αποστάσεων από τα centroids
        int clust_idx = labels[i];
        double dist_sq = squared_euclidean_distance(data[i], centroids[clust_idx]);
        wcss += dist_sq; 
    }

    return wcss;
}
  
//συνάρτηση που κάνει υπολογισμό silhouette score για αξιολόγηση ποιότητας clustering
double k_means :: calc_silhouette(const vector<vector<double>>& data, const vector<int>& labels) const {
    size_t n = data.size();
    if(n<=1)        //δεν ορίζεται για 0 ή 1 σημεία
        return 0.0;

    vector<double> silhouette_scores(n);    //silhouette score για κάθε σημείο

    for(size_t i=0; i<n; i++){  //υπολογισμός μέσης απόστασης στο ίδιο cluster (a_i)
        double a_i = 0.0;
        int c_same = 0;         //πλήθος σημείων στο ίδιο cluster

        for(size_t j=0; j<n; j++){
            if(i!=j && labels[j] == labels[i]){     //ίδιο cluster, διαφορετικό σημείο
                a_i += euclidean_distance(data[i], data[j]);
                c_same++;
            }
        }

        a_i = (c_same > 0) ? a_i / c_same : 0.0;

        //υπολογισμός ελάχιστης μέσης απόστασης από άλλα clusters (b_i)
        double b_i = numeric_limits<double>::max();

        for(int clust_idx=0; clust_idx<k; clust_idx++){
            if(clust_idx == labels[i])  //παράβλεψε το ίδιο cluster
                continue;

            double avg_dist_oth = 0.0;
            int c_oth = 0;  //πλήθος σημείων στο άλλο cluster

            for(size_t j=0; j<n; j++){
                if(labels[j] == clust_idx){
                    avg_dist_oth += euclidean_distance(data[i], data[j]);
                    c_oth++;
                }
            }

            if(c_oth>0){
                avg_dist_oth /= c_oth;
                b_i = min(b_i, avg_dist_oth);   //ελάχιστη μέση απόσταση
            }
        }

        //υπολογισμός silhouette score για το σημείο i
        if(a_i == 0 && b_i == 0)
            silhouette_scores[i] = 0.0;                         //ειδική περίπτωση
        else
            silhouette_scores[i] = (b_i - a_i)/max(a_i, b_i);   //τυπικός τύπος
    }

    //μέσος silhouette score για όλα τα σημεία
    double score = 0.0;
    for(double i : silhouette_scores)
        score += i;

    return score/n;     //κανονικοποίηση
}