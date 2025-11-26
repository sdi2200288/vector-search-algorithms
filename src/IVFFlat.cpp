/* IVFFlat.cpp */

#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <climits>
#include <chrono>
#include <queue> 
#include <set>

#include "../include/IVFFlat.hpp"

using namespace std;

//Constructor
IVFFlat::IVFFlat(int seed1, string input_file1, string query_file1, string output_file1, int kclusters1, int nprobe1, int N1, double R1, string type1, bool range1){
    seed = seed1;
    input_file = input_file1;
    query_file = query_file1;
    output_file = output_file1;
    kclusters = kclusters1;
    N = N1;
    R = R1;
    nprobe = nprobe1;
    type = type1;
    range = range1;

    dimension = (type == "mnist") ? 784 : 128;

    cout << "IVFFlat Constructed!" << endl;
}

//Destructor
IVFFlat :: ~IVFFlat(){
    centroids.clear();
    cluster_members.clear();
    dataset_reference.clear();
    cout << "IVFFlat Deconstructed!" << endl;
}

//συνάρτηση που υπολογίζει την ευκλείδεια απόσταση
double IVFFlat::euclidean_distance(const vector<double>& v1, const vector<double>& v2) {
    double sum = 0.0;
    for (size_t i = 0; i < v1.size(); i++) {
        double diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

//συνάρτηση που αρχικοποιεί δομές δεδομένων για IVFFlat
void IVFFlat::Initialize(size_t dataset_n){
    // cout << "Initialize IVFFlat clusters" << endl;
    
    // Αρχικοποίηση δομών
    centroids.resize(kclusters);        //k κέντρα
    for(int i = 0; i < kclusters; i++) {
        centroids[i].resize(dimension, 0.0);    //κάθε centroid έχει διάσταση 'dimension'
    }
    cluster_members.resize(kclusters);  //k λιστες για τα μελη του καθε cluster
    
    // cout << "IVFFlat components initialized for " << kclusters << " clusters" << endl;
}

//συνάρτηση που κάνει εύρεση nprobe_count πλησιέστερων centroids σε ένα σημείο
vector<int> IVFFlat::find_nearest_centroids(const vector<double>& p, int nprobe_count){
    vector<pair<double,int>> distances; //αποστασεις των centroids

    for(size_t i = 0; i < centroids.size(); i++){   //αποσταση απο καθε centroid
        double dist = euclidean_distance(p,centroids[i]);
        distances.push_back({dist,i});      //αποθήκευση (απόσταση, index)
    }
    
    sort(distances.begin(),distances.end());    //ταξινόμηση κατά αύξουσα απόσταση

    vector<int> result; //επιστροφη των probe_count πλησιετερων centroids
    int count = min(nprobe_count , (int)distances.size());  //μη υοερβαση οριου
    for(int i = 0; i < count; i++){
        result.push_back(distances[i].second);  //προσθήκη index των πλησιέστερων
    }
    return result;
}

//συνάρτηση που κάνει εύρεση του πλησιέστερου centroid σε ένα σημείο
int IVFFlat::find_nearest_centroid(const vector<double>& p){
    int nearest_i = 0;                                          //index του πλησιεστεροθ
    double best_distance = euclidean_distance(p,centroids[0]);  //αποσταση απο το 1ο centroid

    for(size_t i = 1; i < centroids.size(); i++){              //ελεγχος ολων των centroids για να βρεθει ο πλησιεστερος 
        double dist = euclidean_distance(p,centroids[i]);
        //cout << ", " << dist;
        if(dist < best_distance){
            best_distance = dist;                              //ενημερωση καλυτερων
            nearest_i = i;
        }
    }
    return nearest_i;
}

////συνάρτηση που υπολογίζει το silhouette score για ποιότητα clustering
double IVFFlat::calculate_silhouette(){
    // cout << "Calculating Silhouette (this may take a while...)";
    
    // ΧΡΗΣΗ SAMPLE για ταχύτητα
    int sample_size = min(100, (int)dataset_reference.size());
    double total_silhouette = 0.0;
    
    default_random_engine gen(seed);
    uniform_int_distribution<int> dist(0, dataset_reference.size()-1);
    
    for(int s = 0; s < sample_size; s++){
        int idx = dist(gen);    //τυχαίο δείγμα
        const auto& point = dataset_reference[idx];
        
        //εύρεση cluster στο οποίο ανήκει το σημείο
        int cluster_id = -1;
        for(int i = 0; i < kclusters; i++) {
            if(find(cluster_members[i].begin(), cluster_members[i].end(), idx) != cluster_members[i].end()) {
                cluster_id = i;
                break;
            }
        }
        if(cluster_id == -1) continue;
        
        // Υπολογισμός a_i (μέση απόσταση στον ίδιο cluster)
        double a_i = 0.0;
        int same_cluster_count = 0;
        for(int neighbor_idx : cluster_members[cluster_id]){
            if(neighbor_idx != idx) {
                a_i += euclidean_distance(point, dataset_reference[neighbor_idx]);
                same_cluster_count++;
            }
        }
        a_i = (same_cluster_count > 0) ? a_i / same_cluster_count : 0.0;
        
        // Υπολογισμός b_i (ελάχιστη μέση απόσταση από άλλα clusters)
        double b_i = numeric_limits<double>::max();
        for(int other_cluster = 0; other_cluster < kclusters; other_cluster++){
            if(other_cluster == cluster_id) continue;
            
            double dist_to_cluster = 0.0;
            int other_count = 0;
            // Sample από άλλο cluster για ταχύτητα
            int sample_count = min(10, (int)cluster_members[other_cluster].size());
            for(int k = 0; k < sample_count; k++){
                int sample_idx = uniform_int_distribution<int>(0, cluster_members[other_cluster].size()-1)(gen);
                dist_to_cluster += euclidean_distance(point, dataset_reference[cluster_members[other_cluster][sample_idx]]);
                other_count++;
            }
            if(other_count > 0) 
                b_i = min(b_i, dist_to_cluster / other_count);
        }
        
        if(max(a_i, b_i) > 0)   //υπολογισμός silhouette για το τρέχον σημείο
            total_silhouette += (b_i - a_i) / max(a_i, b_i);
    }
    
    return total_silhouette / sample_size;  //μέσος όρος silhouette
}

//συνάρτηση για τη δημιουργία clusters με k-means algorithm
void IVFFlat::CreateClusters(const vector<vector<double>>& dataset){
    // cout << "Creating " << kclusters << " clusters with " << dataset.size() << " vectors" << endl;
    dataset_reference = dataset;
    
    default_random_engine generator(seed);

    // ΚΑΛΥΤΕΡΗ ΕΠΙΛΟΓΗ CENTROIDS - k-means++
    vector<int> selected_indices;
    for(int i = 0; i < kclusters; i++){
        int random_idx;
        int attempts = 0;
        do {
            random_idx = uniform_int_distribution<int>(0, dataset.size()-1)(generator);
            attempts++;
            if(attempts > static_cast<int>(dataset.size())) break; //αποφυγή infinite loop
        } while(find(selected_indices.begin(), selected_indices.end(), random_idx) != selected_indices.end());
        
        selected_indices.push_back(random_idx);
        centroids[i] = dataset[random_idx];     //aρχικοποίηση centroid με τυχαίο σημείο
    }

    //K-MEANS ΜΕ EARLY STOPPING
    for(int iteration = 0; iteration < 20; iteration++){
        //Clear clusters
        for(int i = 0; i < kclusters; i++)
            cluster_members[i].clear();

        //Assign points
        for(int i = 0; i < kclusters; i++)
            cluster_members[i].reserve(dataset.size() / kclusters + 10);
       
        // Αντιστοίχιση σημείων στους πλησιέστερους centroids
        for(size_t j = 0; j < dataset.size(); j++){ 
            int nearest = find_nearest_centroid(dataset[j]);
            cluster_members[nearest].push_back(j);
        }

        // Recalculate centroids
        bool changed = false;
        for(int i = 0; i < kclusters; i++){
            if(cluster_members[i].empty()) continue;
            
            vector<double> new_centroid(dimension, 0.0);
            for(int idx : cluster_members[i])
                for(int d = 0; d < dimension; d++)
                    new_centroid[d] += dataset[idx][d];     //άθροισμα συντεταγμένων
             
            for(int d = 0; d < dimension; d++)
                new_centroid[d] /= cluster_members[i].size();   //μέσος όρος για νέο centroid
            
            //Check for changes
            if(euclidean_distance(centroids[i], new_centroid) > 0.001) 
                changed = true;

            centroids[i] = new_centroid;
        }

        //early stopping αν δεν άλλαξαν τα centroids
        if(iteration > 5 && !changed) {
            // cout << "Converged at iteration " << iteration << endl;
            break;
        }
        
        //Progress reporting
        // if(iteration % 5 == 0) {
        //     cout << "Iteration " << iteration << " - Total points assigned: " << dataset.size() << endl;
        // }
    }

    // Final report
    // cout << "Final cluster sizes: ";
    // for(int i = 0; i < kclusters; i++){
    //     cout << cluster_members[i].size() << " ";
    // }
    // cout << endl;
}

//συνάρτηση που κάνει προσεγγιστική αναζήτηση πλησιέστερων γειτόνων
vector<pair<int, double>> IVFFlat::ANN(const vector<vector<double>>& dataset,const vector<double>& query){

    // ΒΗΜΑ 1: Υπολογισμός απόστασης από όλα τα centroids
    vector<int> candidate_set = find_nearest_centroids(query,nprobe);

    // ΒΗΜΑ 2: Ένωση λιστών ΜΕ RESERVE
    vector<int> candidates;  
    size_t total_size = 0;
    for(int i : candidate_set) 
        total_size += cluster_members[i].size();
    
    candidates.reserve(total_size);
    
    for(int i : candidate_set)
        candidates.insert(candidates.end(), cluster_members[i].begin(), cluster_members[i].end());
    

    // ΒΗΜΑ 3: Χρήση max-heap για τους N πλησιέστερους
    priority_queue<pair<double, int>> max_heap; // (distance, index)
    
    for(int j : candidates){
        double dist_sq = euclidean_distance(query, dataset_reference[j]); // squared distance
        
        if(max_heap.size() < static_cast<size_t>(N)) 
            max_heap.push({dist_sq, j});
        else if(dist_sq < max_heap.top().first) {
            max_heap.pop();
            max_heap.push({dist_sq, j});
        }
    }

    // ΒΗΜΑ 4: Μετατροπή σε sorted vector
    vector<pair<int, double>> result;
    while(!max_heap.empty()) {
        // Πάρε τετραγωνική ρίζα μόνο για τα αποτελέσματα
        result.push_back({max_heap.top().second, sqrt(max_heap.top().first)});
        max_heap.pop();
    }
    // Αντίστροφη σειρά (από μικρότερο σε μεγαλύτερο)
    reverse(result.begin(), result.end());

    return result;
}

//συνάρτηση που κάνει ακριβή αναζήτηση πλησιέστερων γειτόνων
vector<pair<int, double>> IVFFlat::ENN(const vector<vector<double>>& dataset,const vector<double>& query){
    vector<pair<int,double>> all_distances;
    
    //αποσταση απο το query προσ καθε σημειο του dataset
    for(size_t i = 0; i < dataset.size(); i++){
        double dist = euclidean_distance(query,dataset_reference[i]);
        all_distances.push_back({i,dist});
    }
        
    sort(all_distances.begin(),all_distances.end(),[](const auto& a, const auto& b) { return a.second < b.second; });

    //διατηρηση μονο των Ν πλησιεστερων
    if( all_distances.size() > static_cast<size_t>(N))
        all_distances.resize(N);
    
    return all_distances;
}

//συνάρτηση που κάνει προσεγγιστική range search
vector<int> IVFFlat::ARange_Search(const vector<vector<double>>& dataset, const vector<double>& query){
    vector<int> range_neighbors;
    
    // nprobe πλησιέστερους centroids
    vector<int> candidate_centroids = find_nearest_centroids(query, nprobe);
    // cout << "Candidate centroids: " << candidate_centroids.size() << endl;

    // Συλλογή candidates από όλα τα clusters
    vector<int> candidate_set;   
    for(int i : candidate_centroids)
       candidate_set.insert(candidate_set.end(), cluster_members[i].begin(), cluster_members[i].end());

    // Έλεγχος μόνο των candidates - εντός ακτίνας
    int checked = 0;
    int found = 0;
    for(int j : candidate_set){
        double dist = euclidean_distance(query, dataset_reference[j]);
        checked++;
        if(dist <= R){
            range_neighbors.push_back(j);
            found++;
        }
    }
    return range_neighbors;
}

// vector<int> IVFFlat:: ERange_Search( const vector<vector<double>>& dataset, const vector<double>& query){
//     vector<int> range_neighbors;

//     //ελεηχος καθε σημειου του dataset
//     for(size_t j = 0; j < dataset.size(); j++){
//         double dist = euclidean_distance(query,dataset[j]);
//         if(dist <= R){
//             range_neighbors.push_back(j);
//         }
//     }

//     return range_neighbors;
// }

//συνάρτηση που κάνει επεξεργασία όλων των queries και αποθήκευση αποτελεσμάτων
void  IVFFlat::Queries(const vector<vector<double>>& dataset,const vector<vector<double>>& queries){
    // cout << "Calculate IVFFlat Queries" << endl;

    ofstream out(output_file);
    if(!out.is_open()){
        cerr << "Error: Cannot open output file " << output_file << endl;
        return;
    }

    out<<"IVFFlat\n";

    double total_Average_factor = 0.0, total_recall = 0.0;
    double total_atime = 0.0,total_etime = 0.0;
    int valid_q = 0;

    auto start_all = chrono::high_resolution_clock::now();  //χρονομετρο για QPS
    
    //Επεξεργασια καθε query
    for(size_t i = 0; i < queries.size(); i++){
        const auto& q = queries[i]; //τρεχον query

        //Προσεγγιστική αναζήτηση
        auto start = chrono::high_resolution_clock::now();
        auto aNN = ANN(dataset,q);  
        auto end = chrono::high_resolution_clock::now();
        auto atime = chrono::duration<double>(end - start).count();
        
        //Ακριβής αναζήτηση
        start = chrono::high_resolution_clock::now();
        auto eNN = ENN(dataset,q);  
        end = chrono::high_resolution_clock::now();
        auto etime = chrono::duration<double>(end-start).count();
        
        valid_q++;
        total_atime += atime;
        total_etime += etime;
   
        out << "Query: " << i <<endl;

        if(!aNN.empty() && !eNN.empty()) {
            // Nearest neighbors output
            for(size_t neighbor = 0; neighbor < aNN.size(); neighbor++){
                int data_idx =  aNN[neighbor].first;    //index του γειτονα
                
                out << "Nearest neighbor-" << neighbor + 1 << ": " << data_idx << endl ;
                out << "distanceApproximate: " << aNN[neighbor].second << endl;
        
                //εκτυπωση ακριβους αποστασης
                if(neighbor < eNN.size()){
                    out<<"distanceTrue: " << eNN[neighbor].second << endl;
                }
            }

            // Calculate metrics
            double Average_factor = 0.0;    //(AF) - Πόσο καλή είναι η προσέγγιση
            if(eNN[0].second > 0)
                Average_factor = aNN[0].second / eNN[0].second;
            else if(aNN[0].second == 0)
                Average_factor = 1.0;
            
            total_Average_factor += Average_factor;
            
            int recall_hits = 0; // Recall@N - Ποσοστό επιτυχίας στην εύρεση των πραγματικών γειτόνων
            for(const auto& exact : eNN){
                for(const auto& approx : aNN){
                    if(exact.first == approx.first){
                        recall_hits++;
                        break;
                    }
                }
            }
            
            double recall = (double)recall_hits / (double)eNN.size();
            total_recall += recall;
          
            vector<int> aRN, eRN;
            if(range){
                // Approximate range search (για output)
                aRN = ARange_Search(dataset, q);
                                
                out << "R-near neighbors:";
                if(aRN.empty()){
                    out << endl;
                }
                else
                    for(int id: aRN)
                        out << id << endl;
            }

            out<< "Average AF: " << Average_factor << endl;
            out<< "Recall@N: " << recall <<endl;
            out << "tApproximateAverage: " << atime << endl;
            out << "tTrueAverage: " << etime << endl;
            out << "-------------------" << endl;        

        }
    }
    
    //υπολογισμος qps
    auto total_time_all = chrono::high_resolution_clock::now();
    double total_time = chrono::duration<double>(total_time_all - start_all).count();
    double qps = queries.size() / total_time;
    
    out << "QPS: " << qps << endl;
    
    // ΤΕΛΙΚΕΣ ΜΕΤΡΙΚΕΣ
    out << "FINAL METRICS:" << endl;
    out << "Total Average AF: " << total_Average_factor / valid_q << endl;
    out << "Total Average Recall@N: " << total_recall / valid_q << endl;
    out << "QPS: " << qps << endl;
    out << "Average Approximate Time: " << total_atime / valid_q << endl;
    out << "Average Exact Time: " << total_etime / valid_q << endl;

    out.close();
    // cout<<"Complete Queries"<<endl;
}

//συνάρτηση που κάνει την κύρια λειτουργία IVFFlat
void IVFFlat::ivfflat_func(const vector<vector<double>>& dataset, const vector<vector<double>>& queries){
    //έλεγχος εγκυρότητας δεδομένων
    if (dataset.empty()){  
        cerr << "Error: Dataset is empty" << endl;
        return;
    }

    if (queries.empty()){
        cerr << "Error: Query set is empty" << endl;
        return;
    }
    
    // cout << "IVFFlat Algorithm" << endl;
    // cout << "Dataset size: " << dataset.size() << " vectors" << endl;
    // cout << "Vector dimension: " << dataset[0].size() << endl;

    Initialize(dataset.size());     //1.Αρχικοποιηση δομων
    CreateClusters(dataset);        //2. Δημιουργία clusters με k-means
    Queries(dataset,queries);       //3. Εκτέλεση queries και υπολογισμός μετρικών
    
    double silhouette = calculate_silhouette();
    // cout << "Silhouette Score for k=" << kclusters << ": " << silhouette << endl;  
}

//συνάρτηση που εμφανίζει τις παραμέτρους 
void IVFFlat :: print_params() {
    std::cout << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "kclusters : " << kclusters  << ", N: " << N << ", R: " << R << ", nprobe: " << nprobe << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}

// vector<vector<int>> IVFFlat::ComputeKNNGraph(int k) {
//     vector<vector<int>> graph(dataset_reference.size());
    
//     for(size_t i = 0; i < dataset_reference.size(); ++i) {
        
//         vector<pair<double,int>> dist;
//         dist.reserve(dataset_reference.size());

//         for(size_t j = 0; j< dataset_reference.size(); ++j) {
//            if (i == j) continue;
//             double d = euclidean_distance(dataset_reference[i], dataset_reference[j]);
//             dist.push_back({d, (int)j});
//         }

//          // take k smallest
//         nth_element(dist.begin(), dist.begin() + k, dist.end());
//         sort(dist.begin(), dist.begin() + k);
        
//         graph[i].reserve(k);
//         for (int t = 0; t < k; t++)
//             graph[i].push_back(dist[t].second);
//     }
    
//     return graph;
// }

vector<vector<int>> IVFFlat::ComputeKNNGraphApprox(int k) {
    size_t n = dataset_reference.size();
    vector<vector<int>> graph(n);
    
    // Για κάθε σημείο:
    for (size_t i = 0; i < n; ++i) {

        // Step 1: Χρήση των nprobe κοντινών centroids για να περιορίσουμε χώρο αναζήτησης
        vector<int> probe_centroids = find_nearest_centroids(dataset_reference[i], nprobe);

        vector<int> candidates;
        for (int c : probe_centroids) {
            const auto& members = cluster_members[c];
            candidates.insert(candidates.end(), members.begin(), members.end());
        }

        // Step 2: Υπολογισμός απόστασης ΜΟΝΟ με τα candidates
        // (όχι με όλα τα 1M points)
        priority_queue<pair<double,int>> max_heap;

        for (int idx : candidates) {
            if (idx == i) continue;
            double dist = euclidean_distance(dataset_reference[i], dataset_reference[idx]);

            if ((int)max_heap.size() < k)
                max_heap.push({dist, idx});
            else if (dist < max_heap.top().first) {
                max_heap.pop();
                max_heap.push({dist, idx});
            }
        }

        // Step 3: Γέμισμα γραφήματος
        graph[i].resize(max_heap.size());
        int pos = max_heap.size() - 1;
        while (!max_heap.empty()) {
            graph[i][pos--] = max_heap.top().second;
            max_heap.pop();
        }
    }

    return graph;
}
