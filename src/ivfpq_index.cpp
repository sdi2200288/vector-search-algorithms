/* ivfpq_index */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

#include "../include/ivfpq_index.hpp"

using namespace std;

//συνάρτηση που υλοποιεί το index
ivfpq_index :: ivfpq_index(int k_clust1, int M1, int nbits1, int dim1){
    k_clust = k_clust1;
    M = M1;
    nbits = nbits1;
    dim = dim1;

    if(dim % M != 0)    //έλεγχος ότι η διάσταση διαιρείται ακριβώς με τον αριθμό υποδιανυσμάτων
        throw invalid_argument("Dimension must be exactly divisible by M!");

    sub_dim = dim / M;  //υπολογισμός διάστασης κάθε υποδιανύσματος

    cout << "IVFPQ INDEX Constructed: k=" << k_clust << ", M=" << M 
         << ", nbits=" << nbits << ", dim=" << dim << endl;
}

//destructor
ivfpq_index :: ~ivfpq_index(){
    cout << "IVFPQ INDEX Deconstructed!" << endl;
}

//συνάρτηση που υπολογίζει την ευκλείδεια απόσταση
double ivfpq_index :: euclidean_distance(const vector<double>& a, const vector<double>& b) const {
    if(a.size() != b.size())
        throw invalid_argument("Vectors of different dimensions!");

    double dist = 0.0;
    for(size_t i = 0; i < a.size(); i++){
        double diff = a[i] - b[i];
        dist += diff * diff;
    }

    return sqrt(dist);
}

//συνάρτηση που υπολογίζει το υπόλοιπο μεταξύ point και centroid
vector<double> ivfpq_index :: comp_residual(const vector<double>& point, const vector<double>& centroid) const {
    vector<double> residual(point.size());

    for(size_t i = 0; i < point.size(); i++)    //αφαίρεση centroid από το point 
        residual[i] = point[i] - centroid[i];
    
    return residual;
}

//συνάρτηση που κάνει εύρεση nprobe πλησιέστερων centroids σε ένα query
vector<int> ivfpq_index :: find_nearest_centr(const vector<double>& point, int nprobe) const {
    vector<pair<double, int>> dists;    //(απόσταση, index centroid)

    for(int i = 0; i < k_clust; i++){   
        double dist = euclidean_distance(point, ivf_centr[i]);  //υπολογισμός απόστασης από κάθε centroid
        dists.emplace_back(dist, i);    //αποθήκευση (απόσταση, index)
    }

    sort(dists.begin(), dists.end());   //ταξινόμηση κατά αύξουσα απόσταση 

    vector<int> res;
    int probes = min(nprobe, k_clust);  //μη υπέρβαση του αριθμού των clusters
    
    for(int i = 0; i < probes; i++)
        res.push_back(dists[i].second); //προσθήκη index των πλησιέστερων centroids

    return res;
}

//συνάρτηση που δημιουργεί Look-Up Table (LUT) για γρήγορους υπολογισμούς αποστάσεων
vector<vector<double>> ivfpq_index :: build_LUT(const vector<double>& query_residual) const {
    vector<vector<double>> LUT(M);

    for(int i = 0; i < M; i++){
        vector<double> sub_query(sub_dim);      //δημιουργία υποδιανύσματος από το query residual
        for(int j = 0; j < sub_dim; j++)
            sub_query[j] = query_residual[i * sub_dim + j];
        
        LUT[i].resize(1 << nbits);              //αρχικοποίηση LUT για το τρέχον υποδιάνυσμα 2^nbits entries 
        
        for(int j = 0; j < (1 << nbits); j++)   //υπολογισμός απόστασης από όλα τα centroids του PQ codebook
            LUT[i][j] = euclidean_distance(sub_query, pq_codebook[i][j]);
    }

    return LUT;
}

//συνάρτηση που κάνει τη κύρια μέθοδο κατασκευής του IVFPQ ευρετηρίου
void ivfpq_index :: build(const vector<vector<double>>& dataset){
    if(dataset.empty())
        throw invalid_argument("Dataset is empty!");

    // cout << "Building IVFPQ index..." << endl;
    // cout << "Dataset size: " << dataset.size() << " vectors" << endl;  

    //1. Κατασκευή Inverted File
    // cout << "Step 1: IVF clustering with k=" << k_clust << endl;
    k_means ivf_k_means(k_clust, 100, 1e-4, 42);                //k-means για IVF
    vector<int> ivf_labels = ivf_k_means.predict_fit(dataset);  //clustering labels
    ivf_centr = ivf_k_means.get_centroids();                    //αποθήκευση centroids

    //2. Δημιουργία αντεστραμμένων λιστών
    // cout << "Step 2: Creating inverted lists..." << endl;
    inv_list.resize(k_clust);                   //αρχικοποίηση λιστών για κάθε cluster
    for(size_t i = 0; i < dataset.size(); i++)  //αντιστοίχιση κάθε σημείου στο cluster του
        inv_list[ivf_labels[i]].push_back(i);   //προσθήκη index στο cluster

    // 3. Εκπαίδευση Product Quantization
    // cout << "Step 3: PQ training with M=" << M << ", nbits=" << nbits << endl;
    vector<vector<double>> residuals(dataset.size());   //υπολογισμός residuals (διαφορών από τα centroids)
    for(size_t i = 0; i < dataset.size(); i++){
        int centr_idx = ivf_labels[i];
        residuals[i] = comp_residual(dataset[i], ivf_centr[centr_idx]);
    }

    pq_codebook.resize(M);          //αρχικοποίηση PQ codebook
    int pq_clusters = 1 << nbits;   //2^nbits clusters ανά υποχώρο

    for(int i = 0; i < M; i++){     //εκπαίδευση για κάθε ένα από τα M υποδιανύσματα
        // cout << "Training subspace " << (i+1) << "/" << M << endl;
        
        vector<vector<double>> subspace_data;       //δημιουργία δεδομένων για τον τρέχοντα υποχώρο
        for(const auto& resid : residuals){
            vector<double> subvec(sub_dim);
            for(int j = 0; j < sub_dim; j++)
                subvec[j] = resid[i * sub_dim + j]; //εξαγωγή υποδιανύσματος
            
            subspace_data.push_back(subvec);
        }

        //k-means για τον τρέχοντα υποχώρο
        k_means pq_kmeans(pq_clusters, 100, 1e-4, 42 + i);  //διαφορετικό seed για κάθε υποχώρο
        pq_codebook[i] = pq_kmeans.fit(subspace_data);      //αποθήκευση centroids
    }

    //4. Κωδικοποίηση όλων των διανυσμάτων
    // cout << "Step 4: Encoding " << dataset.size() << " vectors..." << endl;
    pq_code.resize(dataset.size(), vector<uint8_t>(M));     //αρχικοποίηση πίνακα κωδικών

    for(size_t i = 0; i < dataset.size(); i++){             //κωδικοποίηση κάθε διανύσματος
        int centr_idx = ivf_labels[i];
        auto residual = comp_residual(dataset[i], ivf_centr[centr_idx]);

        for(int j = 0; j < M; j++){                         //κωδικοποίηση κάθε υποδιανύσματος
            vector<double> sub_vec(sub_dim);
            for(int k = 0; k < sub_dim; k++)
                sub_vec[k] = residual[j * sub_dim + k];     //εξαγωγή υποδιανύσματος


            //εύρεση του καλύτερου κωδικού (πλησιέστερου centroid)
            int best_code = 0;
            double best_dist = numeric_limits<double>::max();   
            
            for(int code = 0; code < pq_clusters; code++){
                double dist = euclidean_distance(sub_vec, pq_codebook[j][code]);
                if(dist < best_dist){
                    best_dist = dist;
                    best_code = code;
                }
            }
            pq_code[i][j] = static_cast<uint8_t>(best_code);    //αποθήκευση κωδικού
        }
    }

    // cout << "IVFPQ index built successfully!" << endl;
}

//συνάρτηση που κάνει αναζήτησης K-NN
vector<pair<double, int>> ivfpq_index :: Query(const vector<double>& query, int nprobe, int N) const {
    if(ivf_centr.empty() || pq_codebook.empty())        //έλεγχος ότι το ευρετήριο έχει κατασκευαστεί
        throw runtime_error("Index not built yet!");

    //εύρεση των nprobe πλησιέστερων centroids
    vector<int> probe_indices = find_nearest_centr(query, nprobe);
    vector<pair<double, int>> candidates;       //(απόσταση, index σημείου)

    //δημιουργία ξεχωριστού LUT για κάθε centroid
    for(int centroid_idx : probe_indices){
        auto query_residual = comp_residual(query, ivf_centr[centroid_idx]);    //υπολογισμός residual για το τρέχον centroid
        vector<vector<double>> LUT = build_LUT(query_residual);

        for(int point_id : inv_list[centroid_idx]){     //έλεγχος όλων των σημείων στο cluster
            double dist = 0.0;
            for(int i = 0; i < M; i++){                 //υπολογισμός απόστασης χρησιμοποιώντας LUT
                uint8_t code = pq_code[point_id][i];    //κωδικός για το υποδιάνυσμα
                dist += LUT[i][code];                   //προσθήκη απόστασης από LUT
            }
            candidates.emplace_back(dist, point_id);    //προσθήκη υποψήφιου
        }
    }

    //ταξινόμηση και περικοπή στους N πλησιέστερους
    sort(candidates.begin(), candidates.end());
    if(candidates.size() > static_cast<size_t>(N))
        candidates.resize(N);

    return candidates;
}

//συνάρτηση που κάνει range search
vector<pair<double, int>> ivfpq_index :: range_search(const vector<double>& query, int nprobe, double R) const {
    if(ivf_centr.empty() || pq_codebook.empty())        //έλεγχος ότι το ευρετήριο έχει κατασκευαστεί
        throw runtime_error("Index not built yet!");
    
    vector<int> probe_indices = find_nearest_centr(query, nprobe);
    vector<pair<double, int>> results;

    //Δημιουργία ξεχωριστού LUT για κάθε centroid
    for(int centroid_idx : probe_indices){
        auto query_residual = comp_residual(query, ivf_centr[centroid_idx]);
        vector<vector<double>> LUT = build_LUT(query_residual);

        for(int point_id : inv_list[centroid_idx]){
            double dist = 0.0;
            for(int i = 0; i < M; i++){
                uint8_t code = pq_code[point_id][i];
                dist += LUT[i][code];
            }

            if(dist <= R)   //προσθήκη μόνο αν η απόσταση είναι ≤ R
                results.emplace_back(dist, point_id);
        }
    }

    sort(results.begin(), results.end());   //ταξινόμηση κατά απόσταση
    return results;
}

//συνάρτηση που κάνει εύρεση του πλησιέστερου γείτονα
pair<double, int> ivfpq_index :: ANN(const vector<double>& query, int nprobe) const {
    auto results = Query(query, nprobe, 1);                     //αναζήτηση μόνο του πλησιέστερου 
    return results.empty() ? make_pair(-1.0, -1) : results[0];  //επιστροφή ή error
}