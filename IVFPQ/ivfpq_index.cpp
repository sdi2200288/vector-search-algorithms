// /* ivfpq_index */

// #include <iostream>
// #include <vector>
// #include <cmath>
// #include <algorithm>

// #include "ivfpq_index.hpp"

// using namespace std;

// ivfpq_index :: ivfpq_index(int k_clust1, int M1, int nbits1, int dim1){
//     k_clust = k_clust1;
//     M = M1;
//     nbits = nbits1;
//     dim = dim1;

//     if(dim%M != 0){
//         throw invalid_argument("Dimension must be exactly divisible by M!");
//     }

//     sub_dim = dim/M;

//     cout << "IVFPQ INDEX Constructed!" << endl;
// }

// ivfpq_index :: ~ivfpq_index(){
//     cout << "IVFPQ INDEX Deconstructed!" << endl;
// }

// double ivfpq_index :: euclidean_distance(const vector<double>& a, const vector<double>& b) const {
//     if(a.size() != b.size()){
//         throw invalid_argument("Vectors of different dimensions!");
//     }

//     double dist = 0.0;
//     for(size_t i=0; i<a.size(); i++){
//         double diff = a[i] - b[i];
//         dist += diff*diff;
//     }

//     return sqrt(dist);
// }

// vector<double> ivfpq_index :: comp_residual(const vector<double>& point, const vector<double>& centroid) const {
//     vector<double> residual(point.size());

//     for(size_t i=0; i<point.size(); i++){
//         residual[i] = point[i] - centroid[i];
//     }

//     return residual;
// }

// vector<int> ivfpq_index :: find_nearest_centr(const vector<double>& point, int nprobe) const {
//     vector<pair<double, int>> dists;

//     for(int i=0; i<k_clust; i++){
//         double dist = euclidean_distance(point, ivf_centr[i]);
//         dists.emplace_back(dist, i);
//     }

//     sort(dists.begin(), dists.end());

//     vector<int> res;
//     int probes = min(nprobe, k_clust);
//     for(int i=0; i<probes; i++){
//         res.push_back(dists[i].second);
//     }

//     return res;
// }

// vector<vector<double>> ivfpq_index :: build_LUT(const vector<double>& query) const {
//     vector<vector<double>> LUT(M);

//     for(int i=0; i<M; i++){
//         vector<double> sub_query(sub_dim);
//         for(int j=0; j<sub_dim; j++){
//             sub_query[j] = query[i*sub_dim + j];
//         }        

//         LUT[i].resize(1 << nbits);
//         for(int j=0; j<(1 << nbits); j++){
//             LUT[i][j] = euclidean_distance(sub_query, pq_codebook[i][j]);
//         }
//     }

//     return LUT;
// }

// void ivfpq_index :: build(const vector<vector<double>>& dataset){
//     if(dataset.empty()){
//         throw invalid_argument("Dataset is empty!");
//     }

//     cout << "Building IVFPQ index...." << endl;
//     cout << "Dataset size: " << dataset.size() << " vectors" << endl;  

//     cout << "Step 1: IVF clustering with k: " << k_clust << endl;
//     k_means ivf_k_means(k_clust, 100, 1e-4, 42);

//     vector<int> ivf_labels = ivf_k_means.predict_fit(dataset);
//     ivf_centr = ivf_k_means.get_centroids();

//     cout << "Step 2: Creating inverted lists..." << endl;
//     inv_list.resize(k_clust);

//     for(size_t i=0; i<dataset.size(); i++){
//         inv_list[ivf_labels[i]].push_back(i);
//     }

//     for(int i=0; i<k_clust; i++){
//         if(inv_list[i].empty()){
//             cout << "Warning: Cluster " << i << " is empty!" << endl;
//         }
//     }

//     cout << "Step 3: PQ training with M=" << M << ", nbits=" << nbits << endl;

//     vector<vector<double>> residual(dataset.size());
//     for(size_t i=0; i<dataset.size(); i++){
//         int centr_idx = ivf_labels[i];
//         residual[i] = comp_residual(dataset[i], ivf_centr[centr_idx]);
//     }

//     pq_codebook.resize(M);
//     int pq_clust = 1 << nbits;  // 2^nbits

//     for(int i=0; i<M; i++){
//         cout << "Training subspace " << i+1 << "/" << M << " (clusters: " << pq_clust << ")" << endl;
    
//         vector<vector<double>> subspace_data;
//         for(const auto& resid : residual){
//             vector<double> subvec(sub_dim);
//             for(int k=0; k<sub_dim; k++){
//                 subvec[k] = resid[i*sub_dim + k];
//             }
//             subspace_data.push_back(subvec);
//         }

//         k_means pq_k_means(pq_clust, 50, 1e-4, 42+i);
//         pq_codebook[i] = pq_k_means.fit(subspace_data);
//     }

//     cout << "Step 4: Encoding " << dataset.size() << " vectors..." << endl;
//     pq_code.resize(dataset.size(), vector<uint8_t>(M));

//     for(size_t i=0; i<dataset.size(); i++){
//         int centr_idx = ivf_labels[i];
//         auto resid = comp_residual(dataset[i], ivf_centr[centr_idx]);

//         for(int j=0; j<M; j++){
//             vector<double> sub_vec(sub_dim);
//             for(int k=0; k<sub_dim; k++){
//                 sub_vec[k] = resid[j*sub_dim + k];
//             }

//             int best_code = 0;
//             double best_dist = numeric_limits<double>::max();
            
//             for(int c=0; c<pq_clust; c++){
//                 double dist = euclidean_distance(sub_vec, pq_codebook[j][c]);
//                 if(dist<best_dist){
//                     best_dist = dist;
//                     best_code = c;
//                 }
//             }

//             pq_code[i][j] = static_cast<uint8_t>(best_code);
//         }

//         if(dataset.size()>1000 && i%1000==0){
//             cout << "Encoded " << i << "/" << dataset.size() << " vectors" << endl;
//         }
//     }

//     cout << "IVFPQ index built successfully!" << endl;
//     cout << " - IVF clusters: " << k_clust << endl;
//     cout << " - PQ subspaces: " << M << endl;
//     cout << " - Codes per subspace: " << pq_clust << endl;
//     cout << " - Vectors encoded: " << dataset.size() << endl;
// }

// vector<pair<double, int>> ivfpq_index :: Query(const vector<double>& query, int nprobe, int N) const {
//     if(ivf_centr.empty() || pq_codebook.empty()){
//         throw runtime_error("Index not built yet!");
//     }

//     vector<int> probe_indices = find_nearest_centr(query, nprobe);
//     vector<pair<double, int>> cand;

//     int best_centroid_idx = probe_indices[0]; 
//     auto query_residual = comp_residual(query, ivf_centr[best_centroid_idx]);
//     vector<vector<double>> LUT = build_LUT(query_residual);

//     for(int centr_idx : probe_indices){
//         for(int point_id : inv_list[centr_idx]){
//             double dist = 0.0;
//             for(int i=0; i<M; i++){
//                 uint8_t code = pq_code[point_id][i];
//                 dist += LUT[i][code];
//             }

//             cand.emplace_back(dist, point_id);
//         }
//     }

//     sort(cand.begin(), cand.end());
//     if(cand.size() > static_cast<size_t>(N)){
//         cand.resize(N);
//     }

//     return cand;
// }

// vector<pair<double, int>> ivfpq_index :: range_search(const vector<double>& query, int nprobe, double R) const {
//     if(ivf_centr.empty() || pq_codebook.empty()){
//         throw runtime_error("Index not built yet!");
//     }

//     vector<int> probe_indices = find_nearest_centr(query, nprobe);
    
//     int best_centroid_idx = probe_indices[0];
//     auto query_residual = comp_residual(query, ivf_centr[best_centroid_idx]);
//     vector<vector<double>> LUT = build_LUT(query_residual);

//     vector<pair<double, int>> res;

//     for(int centr_idx : probe_indices){
//         for(int point_id : inv_list[centr_idx]){
//             double dist = 0.0;
//             for(int i=0; i<M; i++){
//                 uint8_t code = pq_code[point_id][i];
//                 dist += LUT[i][code];
//             }

//             if(dist <= R){
//                 res.emplace_back(dist, point_id);
//             }
//         }
//     }

//     sort(res.begin(), res.end());

//     return res;
// }

// pair<double, int> ivfpq_index :: ANN(const vector<double>& query, int nprobe) const {
//     auto res = Query(query, nprobe, 1);
//     return res.empty() ? make_pair(-1.0, -1) : res[0];
// }


/* ivfpq_index */

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "ivfpq_index.hpp"

using namespace std;

ivfpq_index :: ivfpq_index(int k_clust1, int M1, int nbits1, int dim1){
    k_clust = k_clust1;
    M = M1;
    nbits = nbits1;
    dim = dim1;

    if(dim % M != 0){
        throw invalid_argument("Dimension must be exactly divisible by M!");
    }

    sub_dim = dim / M;
    cout << "IVFPQ INDEX Constructed: k=" << k_clust << ", M=" << M 
         << ", nbits=" << nbits << ", dim=" << dim << endl;
}

ivfpq_index :: ~ivfpq_index(){
    cout << "IVFPQ INDEX Deconstructed!" << endl;
}

double ivfpq_index :: euclidean_distance(const vector<double>& a, const vector<double>& b) const {
    if(a.size() != b.size()){
        throw invalid_argument("Vectors of different dimensions!");
    }

    double dist = 0.0;
    for(size_t i = 0; i < a.size(); i++){
        double diff = a[i] - b[i];
        dist += diff * diff;
    }
    return sqrt(dist);
}

vector<double> ivfpq_index :: comp_residual(const vector<double>& point, const vector<double>& centroid) const {
    vector<double> residual(point.size());
    for(size_t i = 0; i < point.size(); i++){
        residual[i] = point[i] - centroid[i];
    }
    return residual;
}

vector<int> ivfpq_index :: find_nearest_centr(const vector<double>& point, int nprobe) const {
    vector<pair<double, int>> dists;

    // ΔΙΟΡΘΩΣΗ: int i αντί για size_t
    for(int i = 0; i < k_clust; i++){
        double dist = euclidean_distance(point, ivf_centr[i]);
        dists.emplace_back(dist, i);
    }

    sort(dists.begin(), dists.end());

    vector<int> res;
    int probes = min(nprobe, k_clust);
    // ΔΙΟΡΘΩΣΗ: int i αντί για size_t
    for(int i = 0; i < probes; i++){
        res.push_back(dists[i].second);
    }

    return res;
}

/*vector<vector<double>> ivfpq_index :: build_LUT(const vector<double>& query_residual) const {
    vector<vector<double>> LUT(M);

    for(int i = 0; i < M; i++){
        vector<double> sub_query(sub_dim);
        for(int j = 0; j < sub_dim; j++){
            // ΔΙΟΡΘΩΣΗ: Χρησιμοποίησε το query_residual
            sub_query[j] = query_residual[i * sub_dim + j];
        }        

        LUT[i].resize(1 << nbits);
        for(int j = 0; j < (1 << nbits); j++){
            LUT[i][j] = euclidean_distance(sub_query, pq_codebook[i][j]);
        }
    }

    return LUT;
}*/

vector<vector<double>> ivfpq_index :: build_LUT(const vector<double>& query_residual) const {
    vector<vector<double>> LUT(M);

    for(int i = 0; i < M; i++){
        vector<double> sub_query(sub_dim);
        for(int j = 0; j < sub_dim; j++){
            // ΔΙΟΡΘΩΣΗ: Χρησιμοποίησε το query_residual
            sub_query[j] = query_residual[i * sub_dim + j];
        }        

        LUT[i].resize(1 << nbits);
        for(int j = 0; j < (1 << nbits); j++){
            LUT[i][j] = euclidean_distance(sub_query, pq_codebook[i][j]);
        }
    }

    return LUT;
}

void ivfpq_index :: build(const vector<vector<double>>& dataset){
    if(dataset.empty()){
        throw invalid_argument("Dataset is empty!");
    }

    cout << "Building IVFPQ index..." << endl;
    cout << "Dataset size: " << dataset.size() << " vectors" << endl;  

    // 1. IVF CLUSTERING
    cout << "Step 1: IVF clustering with k=" << k_clust << endl;
    k_means ivf_k_means(k_clust, 100, 1e-4, 42);
    vector<int> ivf_labels = ivf_k_means.predict_fit(dataset);
    ivf_centr = ivf_k_means.get_centroids();

    // 2. INVERTED LISTS
    cout << "Step 2: Creating inverted lists..." << endl;
    inv_list.resize(k_clust);
    for(size_t i = 0; i < dataset.size(); i++){
        inv_list[ivf_labels[i]].push_back(i);
    }

    // 3. PQ TRAINING
    cout << "Step 3: PQ training with M=" << M << ", nbits=" << nbits << endl;
    vector<vector<double>> residuals(dataset.size());
    for(size_t i = 0; i < dataset.size(); i++){
        int centr_idx = ivf_labels[i];
        residuals[i] = comp_residual(dataset[i], ivf_centr[centr_idx]);
    }

    pq_codebook.resize(M);
    int pq_clusters = 1 << nbits;

    for(int i = 0; i < M; i++){
        cout << "Training subspace " << (i+1) << "/" << M << endl;
        
        vector<vector<double>> subspace_data;
        for(const auto& resid : residuals){
            vector<double> subvec(sub_dim);
            for(int j = 0; j < sub_dim; j++){
                subvec[j] = resid[i * sub_dim + j];
            }
            subspace_data.push_back(subvec);
        }

        k_means pq_kmeans(pq_clusters, 100, 1e-4, 42 + i);
        pq_codebook[i] = pq_kmeans.fit(subspace_data);
    }

    // 4. ENCODING
    cout << "Step 4: Encoding " << dataset.size() << " vectors..." << endl;
    pq_code.resize(dataset.size(), vector<uint8_t>(M));

    for(size_t i = 0; i < dataset.size(); i++){
        int centr_idx = ivf_labels[i];
        auto residual = comp_residual(dataset[i], ivf_centr[centr_idx]);

        for(int j = 0; j < M; j++){
            vector<double> sub_vec(sub_dim);
            for(int k = 0; k < sub_dim; k++){
                sub_vec[k] = residual[j * sub_dim + k];
            }

            int best_code = 0;
            double best_dist = numeric_limits<double>::max();
            
            for(int code = 0; code < pq_clusters; code++){
                double dist = euclidean_distance(sub_vec, pq_codebook[j][code]);
                if(dist < best_dist){
                    best_dist = dist;
                    best_code = code;
                }
            }
            pq_code[i][j] = static_cast<uint8_t>(best_code);
        }
    }

    cout << "IVFPQ index built successfully!" << endl;
}

/*vector<pair<double, int>> ivfpq_index :: Query(const vector<double>& query, int nprobe, int N) const {
    if(ivf_centr.empty() || pq_codebook.empty()){
        throw runtime_error("Index not built yet!");
    }

    vector<int> probe_indices = find_nearest_centr(query, nprobe);
    vector<pair<double, int>> candidates;

    // ΔΙΟΡΘΩΣΗ: Δημιούργησε LUT για ΚΑΘΕ centroid ξεχωριστά
    for(int centroid_idx : probe_indices){
        auto query_residual = comp_residual(query, ivf_centr[centroid_idx]);
        vector<vector<double>> LUT = build_LUT(query_residual);

        for(int point_id : inv_list[centroid_idx]){
            double dist = 0.0;
            for(int i = 0; i < M; i++){
                uint8_t code = pq_code[point_id][i];
                dist += LUT[i][code];
            }
            candidates.emplace_back(dist, point_id);
        }
    }

    sort(candidates.begin(), candidates.end());
    if(candidates.size() > static_cast<size_t>(N)){
        candidates.resize(N);
    }

    return candidates;
}*/

vector<pair<double, int>> ivfpq_index :: Query(const vector<double>& query, int nprobe, int N) const {
    if(ivf_centr.empty() || pq_codebook.empty()){
        throw runtime_error("Index not built yet!");
    }

    vector<int> probe_indices = find_nearest_centr(query, nprobe);
    vector<pair<double, int>> candidates;

    // ΔΙΟΡΘΩΣΗ: Δημιουργία ξεχωριστού LUT για ΚΑΘΕ centroid
    for(int centroid_idx : probe_indices){
        auto query_residual = comp_residual(query, ivf_centr[centroid_idx]);
        vector<vector<double>> LUT = build_LUT(query_residual);

        for(int point_id : inv_list[centroid_idx]){
            double dist = 0.0;
            for(int i = 0; i < M; i++){
                uint8_t code = pq_code[point_id][i];
                dist += LUT[i][code];
            }
            candidates.emplace_back(dist, point_id);
        }
    }

    sort(candidates.begin(), candidates.end());
    if(candidates.size() > static_cast<size_t>(N)){
        candidates.resize(N);
    }

    return candidates;
}

/*vector<pair<double, int>> ivfpq_index :: range_search(const vector<double>& query, int nprobe, double R) const {
    if(ivf_centr.empty() || pq_codebook.empty()){
        throw runtime_error("Index not built yet!");
    }

    vector<int> probe_indices = find_nearest_centr(query, nprobe);
    vector<pair<double, int>> results;

    // ΔΙΟΡΘΩΣΗ: Δημιούργησε LUT για ΚΑΘΕ centroid ξεχωριστά
    for(int centroid_idx : probe_indices){
        auto query_residual = comp_residual(query, ivf_centr[centroid_idx]);
        vector<vector<double>> LUT = build_LUT(query_residual);

        for(int point_id : inv_list[centroid_idx]){
            double dist = 0.0;
            for(int i = 0; i < M; i++){
                uint8_t code = pq_code[point_id][i];
                dist += LUT[i][code];
            }

            if(dist <= R){
                results.emplace_back(dist, point_id);
            }
        }
    }

    sort(results.begin(), results.end());
    return results;
}*/

vector<pair<double, int>> ivfpq_index :: range_search(const vector<double>& query, int nprobe, double R) const {
    if(ivf_centr.empty() || pq_codebook.empty()){
        throw runtime_error("Index not built yet!");
    }

    vector<int> probe_indices = find_nearest_centr(query, nprobe);
    vector<pair<double, int>> results;

    // ΔΙΟΡΘΩΣΗ: Δημιουργία ξεχωριστού LUT για ΚΑΘΕ centroid
    for(int centroid_idx : probe_indices){
        auto query_residual = comp_residual(query, ivf_centr[centroid_idx]);
        vector<vector<double>> LUT = build_LUT(query_residual);

        for(int point_id : inv_list[centroid_idx]){
            double dist = 0.0;
            for(int i = 0; i < M; i++){
                uint8_t code = pq_code[point_id][i];
                dist += LUT[i][code];
            }

            if(dist <= R){
                results.emplace_back(dist, point_id);
            }
        }
    }

    sort(results.begin(), results.end());
    return results;
}

pair<double, int> ivfpq_index :: ANN(const vector<double>& query, int nprobe) const {
    auto results = Query(query, nprobe, 1);
    return results.empty() ? make_pair(-1.0, -1) : results[0];
}