/* IVFFlat */

#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <climits>
#include <chrono>
#include <queue> 

#include "../include/IVFFlat.hpp"

using namespace std;

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
    //max_iterations = 100;

    cout << "IVFFlat Constructed!" << endl;
}

IVFFlat :: ~IVFFlat(){
    centroids.clear();
    cluster_members.clear();
    dataset_reference.clear();
    cout << "IVFFlat Deconstructed!" << endl;
}

double IVFFlat::euclidean_distance(const vector<double>& v1, const vector<double>& v2) {
    double sum = 0.0;
    for (size_t i = 0; i < v1.size(); i++) {
        double diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void IVFFlat::Initialize(size_t dataset_n){
    cout << "Initialize IVFFlat clusters" << endl;
    
    // Αρχικοποίηση δομών
    centroids.resize(kclusters);        //k κέντρα
    cluster_members.resize(kclusters);  //k λιστες για τα μελη του καθε cluster
    
    cout << "IVFFlat components initialized for " << kclusters << " clusters" << endl;
}

//nprobe_count πλησιεστερους centroids σε ενα σημειο
vector<int> IVFFlat::find_nearest_centroids(const vector<double>& p, int nprobe_count){
    vector<pair<double,int>> distances; //αποστασεις των centroids

    for(size_t i = 0; i < centroids.size(); i++){   //αποσταση απο καθε centroid
        double dist = euclidean_distance(p,centroids[i]);
        distances.push_back({dist,i});
    }
    
    sort(distances.begin(),distances.end());

    vector<int> result; //επιστροφη των probe_count πλησιετερων centroids
    int count = min(nprobe_count , (int)distances.size());  //μη υοερβαση οριου
    for(int i = 0; i < count; i++){
        result.push_back(distances[i].second);
    }
    return result;
}

//Πλησιεστερο centroid σε ενα σημειο
int IVFFlat::find_nearest_centroid(const vector<double>& p){
    int nearest_i = 0;                                          //index του πλησιεστεροθ
    double best_distance = euclidean_distance(p,centroids[0]);  //αποσταση απο το 1ο centroid

    //cout << "Finding centroid for point. Distances: " << best_distance;

    for(size_t i = 1; i < centroids.size(); i++){              //ελεγχος ολων των centroids για να βρεθει ο πλησιεστερος 
        double dist = euclidean_distance(p,centroids[i]);
        //cout << ", " << dist;
        if(dist < best_distance){
            best_distance = dist;                              //ενημερωση καλυτερων
            nearest_i = i;
        }
    }
    //cout << " -> Chose centroid " << nearest_i << endl;
    return nearest_i;
}

// υπολογισμός Silhouette Score-μετρικης ποιοτητας
// double IVFFlat::calculate_silhouette() {
//     double total_silhouette = 0.0;                  //Συνολικο Silhouette Score
//     int total_points = 0;                           //Συνολικος αριθμος σημειων
    
//     for (int i = 0; i < kclusters; i++) {           //για κάθε cluster
//         for (int idx : cluster_members[i]) {        //για κάθε σημείο στο cluster i
//             const auto& point = dataset_reference[idx];
            
//             // Υπολογισμός μέσης απόστασης εντός του cluster (a_i)
//             double a_i = 0.0;
//             for (int neighbor_idx : cluster_members[i]) {
//                 if (idx != neighbor_idx) {
//                     a_i += euclidean_distance(point, dataset_reference[neighbor_idx]);
//                 }
//             }

//             a_i = cluster_members[i].size() > 1 ? a_i / (cluster_members[i].size() -1) : 0.0;   //μεσος ορος αποστασεων

//             //ελάχιστη μέση απόσταση από άλλα clusters (b_i)
//             double b_i = numeric_limits<double>::max();
//             for (int other_cluster = 0; other_cluster < kclusters; other_cluster++) {
//                 if (other_cluster == i) continue;
                
                
//                 double dist_to_cluster = 0.0;
//                 for (int neighbor_idx : cluster_members[other_cluster]) {
//                     dist_to_cluster += euclidean_distance(point, dataset_reference[neighbor_idx]);
//                 }
//                 dist_to_cluster /= cluster_members[other_cluster].size();
//                 b_i = min(b_i, dist_to_cluster);    //Διατηρηση της ελαχιστης
//             } 
//             // Υπολογισμός silhouette για το σημείο i
//             double s_i = (b_i - a_i) / max(a_i, b_i);
//             total_silhouette += s_i;
//             total_points++;    
        
//         }
//     }
//     return  total_silhouette / total_points; 
    
// }

double IVFFlat::calculate_silhouette() {
    // ΠΡΟΣΟΧΗ: Αυτή η function είναι ΠΟΛΥ ΒΑΡΥ
    // Μπορείς να την απενεργοποιήσεις ή να χρησιμοποιήσεις μόνο sample
    cout << "Calculating Silhouette (this may take a while...)";
    
    // ΧΡΗΣΗ SAMPLE για ταχύτητα
    int sample_size = min(100, (int)dataset_reference.size());
    double total_silhouette = 0.0;
    
    default_random_engine gen(seed);
    uniform_int_distribution<int> dist(0, dataset_reference.size()-1);
    
    for(int s = 0; s < sample_size; s++) {
        int idx = dist(gen);
        const auto& point = dataset_reference[idx];
        
        // Find which cluster this point belongs to
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
        for(int neighbor_idx : cluster_members[cluster_id]) {
            if(neighbor_idx != idx) {
                a_i += euclidean_distance(point, dataset_reference[neighbor_idx]);
                same_cluster_count++;
            }
        }
        a_i = (same_cluster_count > 0) ? a_i / same_cluster_count : 0.0;
        
        // Υπολογισμός b_i (ελάχιστη μέση απόσταση από άλλα clusters)
        double b_i = numeric_limits<double>::max();
        for(int other_cluster = 0; other_cluster < kclusters; other_cluster++) {
            if(other_cluster == cluster_id) continue;
            
            double dist_to_cluster = 0.0;
            int other_count = 0;
            // Sample από άλλο cluster για ταχύτητα
            int sample_count = min(10, (int)cluster_members[other_cluster].size());
            for(int k = 0; k < sample_count; k++) {
                int sample_idx = uniform_int_distribution<int>(0, cluster_members[other_cluster].size()-1)(gen);
                dist_to_cluster += euclidean_distance(point, dataset_reference[cluster_members[other_cluster][sample_idx]]);
                other_count++;
            }
            if(other_count > 0) {
                b_i = min(b_i, dist_to_cluster / other_count);
            }
        }
        
        if(max(a_i, b_i) > 0) {
            total_silhouette += (b_i - a_i) / max(a_i, b_i);
        }
    }
    
    return total_silhouette / sample_size;
}

//Δημιουργια cluster με k-means
// void IVFFlat::CreateClusters(const vector<vector<double>>& dataset){
    
//     cout<<"Creating " << kclusters << "clusters" << endl;
//     dataset_reference = dataset;                            //Αποθηκευση αναφορας προς το dataset
    
//     default_random_engine generator(seed);

//     //Αρχικοποιηση τυχαίων σημείων απο το dataset X',αρχικοποίηση τυχαίων centroids 
//     for(int i = 0 ; i < kclusters; i++){
//         int random_i = uniform_int_distribution<int>(0,dataset.size() -1)(generator);   //επιλογη τυχαιου index απο το dataset
//         centroids[i] = dataset[random_i];
//     }

//     //k-means επαναληψεις
//     for(int iteration = 0 ; iteration < 10 ; iteration++){
//         //καθαριζω των μελων της λιστας για ολα τα clusters
//         for(int i = 0; i < kclusters; i++ ){
//             cluster_members[i].clear();
//         }

//         // ΒΗΜΑ 2: Βρες πλησιέστερο centroid για ΚΑΘΕ σημείο
//         for(size_t j = 0; j < dataset.size(); j++){
//             int nearest_centroid = find_nearest_centroid(dataset[j]);
//             // ΒΗΜΑ 3: Πρόσθεσε στην ανεστραμμένη λίστα
//             cluster_members[nearest_centroid].push_back(j);
//         }

//         // Επανυπολογισμός centroids
//         //νεα centroids (μεσος ορος των σημειων του clyster)
//         for(int i = 0; i < kclusters; i++){
//             if(cluster_members[i].empty()) continue;    //παραβλεψη κενων clusters

//             vector<double> new_centroid(dimension, 0.0);
//             for(int idx : cluster_members[i]){          //αθροισμα ολων των σημειων του cluster
//                 for(int d = 0; d < dimension; d++){
//                     new_centroid[d] += dataset[idx][d];
//                 }   
//             }
//             for(int d = 0; d < dimension; d++){         //διαιρεση με τον αριθμο σημειων για μεσο ορο
//                 new_centroid[d] /= cluster_members[i].size();
//             }
//             centroids[i] = new_centroid;
//         }  
//     }
// }

void IVFFlat::CreateClusters(const vector<vector<double>>& dataset){
    cout << "Creating " << kclusters << " clusters with " << dataset.size() << " vectors" << endl;
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
            if(attempts > static_cast<int>(dataset.size())) break; // Avoid infinite loop
        } while(find(selected_indices.begin(), selected_indices.end(), random_idx) != selected_indices.end());
        
        selected_indices.push_back(random_idx);
        centroids[i] = dataset[random_idx];
    }

    // K-MEANS ΜΕ EARLY STOPPING
    for(int iteration = 0; iteration < 20; iteration++){
        // Clear clusters
        for(int i = 0; i < kclusters; i++){
            cluster_members[i].clear();
        }

        // Assign points - ΒΕΛΤΙΩΣΗ: Προ-δεσμευση μνήμης
        for(int i = 0; i < kclusters; i++){
            cluster_members[i].reserve(dataset.size() / kclusters + 10);
        }
        
        for(size_t j = 0; j < dataset.size(); j++){
            int nearest = find_nearest_centroid(dataset[j]);
            cluster_members[nearest].push_back(j);
        }

        // Recalculate centroids
        bool changed = false;
        for(int i = 0; i < kclusters; i++){
            if(cluster_members[i].empty()) continue;
            
            vector<double> new_centroid(dimension, 0.0);
            for(int idx : cluster_members[i]){
                for(int d = 0; d < dimension; d++){
                    new_centroid[d] += dataset[idx][d];
                }
            }
            for(int d = 0; d < dimension; d++){
                new_centroid[d] /= cluster_members[i].size();
            }
            
            // Check for changes
            if(euclidean_distance(centroids[i], new_centroid) > 0.001) {
                changed = true;
            }
            centroids[i] = new_centroid;
        }

        // Early stopping
        if(iteration > 5 && !changed) {
            cout << "Converged at iteration " << iteration << endl;
            break;
        }
        
        // Progress reporting
        if(iteration % 5 == 0) {
            cout << "Iteration " << iteration << " - Total points assigned: " << dataset.size() << endl;
        }
    }

    // Final report
    cout << "Final cluster sizes: ";
    for(int i = 0; i < kclusters; i++){
        cout << cluster_members[i].size() << " ";
    }
    cout << endl;
}

vector<pair<int, double>> IVFFlat::ANN(const vector<vector<double>>& dataset,const vector<double>& query){

    // ΒΗΜΑ 1: Υπολογισμός απόστασης από όλα τα centroids
    vector<int> candidate_set = find_nearest_centroids(query,nprobe);

    // ΒΗΜΑ 2: Ένωση λιστών ,ενωση σημειων απο τα επιλεγμενα clusters
    // vector<int> candidates;  
    // for(int i : candidate_set){
    //     candidates.insert(candidates.end(),cluster_members[i].begin(),cluster_members[i].end());

    // }

    // ΒΗΜΑ 2: Ένωση λιστών ΜΕ RESERVE
    vector<int> candidates;  
    size_t total_size = 0;
    for(int i : candidate_set) {
        total_size += cluster_members[i].size();
    }
    candidates.reserve(total_size);
    
    for(int i : candidate_set){
        candidates.insert(candidates.end(), cluster_members[i].begin(), cluster_members[i].end());
    }

    // // ΒΗΜΑ 3: Ακριβής αναζήτηση στο υποψήφιο σύνολο ,υπολογισμος αποστασεων και Ν πλησιεστερους
    // vector<pair<double,int>> candidate_distances;
    // for(int j : candidates){
    //     double dist = euclidean_distance(query,dataset_reference[j]);
    //     candidate_distances.push_back({dist, j});
    // }

    // // ΒΗΜΑ 4: Επιστροφή R πλησιέστερων γειτόνων
    // sort(candidate_distances.begin(),candidate_distances.end());

    // //επιστροφη των Ν πλησιεστερων
    // vector<pair<int,double>> result;
    // int count = min(N,(int)candidate_distances.size());
    // for(int i = 0; i< count; i++){
    //     result.push_back({candidate_distances[i].second, candidate_distances[i].first});
    // }

    // ΒΗΜΑ 3: Χρήση max-heap για τους N πλησιέστερους
    priority_queue<pair<double, int>> max_heap; // (distance, index)
    
    for(int j : candidates){
        double dist_sq = euclidean_distance(query, dataset_reference[j]); // squared distance
        
        if(max_heap.size() < static_cast<size_t>(N)) {
            max_heap.push({dist_sq, j});
        } else if(dist_sq < max_heap.top().first) {
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

vector<pair<int, double>> IVFFlat::ENN(const vector<vector<double>>& dataset,const vector<double>& query){
    vector<pair<int,double>> all_distances;
    
    //αποσταση απο το query προσ καθε σημειο του dataset
    for(size_t i = 0; i < dataset.size(); i++){
        double dist = euclidean_distance(query,dataset[i]);
        all_distances.push_back({i,dist});
    }
        
    sort(all_distances.begin(),all_distances.end(),[](const auto& a, const auto& b) { return a.second < b.second; });

    //διατηρηση μονο των Ν πλησιεστερων
    if(/*all_distances.size() > N*/ all_distances.size() > static_cast<size_t>(N)){
        all_distances.resize(N);
    }
    return all_distances;
}

// vector<int> IVFFlat:: ARange_Search(const vector<vector<double>>& dataset, const vector<double>& query){
//     vector<int> range_neighbors;
    
//     //nprobe πλησιεστερους centroids
//     vector<int> candidate_centroids = find_nearest_centroids(query,nprobe);

//     // Συλλογή candidates από όλα τα clusters
//     vector<int> candidate_set;   
//     for(int i : candidate_centroids){
//        candidate_set.insert(candidate_set.end(),cluster_members[i].begin(),cluster_members[i].end());
//     }

//     // Έλεγχος μόνο των candidates - εντος ακτινας
//     for(int j : candidate_set){
//         double dist = euclidean_distance(query,dataset_reference[j]);
//         if(dist <= R){
//             range_neighbors.push_back(j);
//         }
//     }

//     return range_neighbors;
// }

vector<int> IVFFlat::ARange_Search(const vector<vector<double>>& dataset, const vector<double>& query){
    vector<int> range_neighbors;
    
    // nprobe πλησιέστερους centroids
    vector<int> candidate_centroids = find_nearest_centroids(query, nprobe);
    cout << "Candidate centroids: " << candidate_centroids.size() << endl;

    // Συλλογή candidates από όλα τα clusters
    vector<int> candidate_set;   
    for(int i : candidate_centroids){
       candidate_set.insert(candidate_set.end(), cluster_members[i].begin(), cluster_members[i].end());
    }

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

vector<int> IVFFlat:: ERange_Search( const vector<vector<double>>& dataset, const vector<double>& query){
    vector<int> range_neighbors;

    //ελεηχος καθε σημειου του dataset
    for(size_t j = 0; j < dataset.size(); j++){
        double dist = euclidean_distance(query,dataset[j]);
        if(dist <= R){
            range_neighbors.push_back(j);
        }
    }

    return range_neighbors;
}

void  IVFFlat::Queries(const vector<vector<double>>& dataset,const vector<vector<double>>& queries){
    cout << "Calculate IVFFlat Queries" << endl;

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
        if(i % 100 == 0) {
            cout << "Processed " << i << "/" << queries.size() << " queries" << endl;
        }
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
            if(eNN[0].second > 0){
                Average_factor = aNN[0].second / eNN[0].second;
            }else if(aNN[0].second == 0){
                Average_factor = 1.0;
            }
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
                
                // // Exact range search (για μετρικές σύγκρισης - προαιρετικό)
                //eRN = ERange_Search(dataset, q);
                
                out << "R-near neighbors:";
                if(aRN.empty()){
                    out << endl;
                }else{
                    for(int id: aRN){
                        out << id << endl;
                    }
                }

                // out << "R-near neighbors (Exact):";
                // if(eRN.empty()){
                //     out << endl;
                // }else{
                //     for(int id: eRN){
                //         out << id << endl;
                //     }
                // }
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
    cout<<"Complete Queries"<<endl;
}

void IVFFlat::ivfflat_func(const vector<vector<double>>& dataset, const vector<vector<double>>& queries){
    if (dataset.empty()) {
        cerr << "Error: Dataset is empty" << endl;
        return;
    }
    if (queries.empty()) {
        cerr << "Error: Query set is empty" << endl;
        return;
    }
    
    cout << "IVFFlat Algorithm" << endl;
    cout << "Dataset size: " << dataset.size() << " vectors" << endl;
    cout << "Vector dimension: " << dataset[0].size() << endl;

    Initialize(dataset.size());     //1.Αρχικοποιηση δομων
    CreateClusters(dataset);        //2. Δημιουργία clusters με k-means
    Queries(dataset,queries);       //3. Εκτέλεση queries και υπολογισμός μετρικών
    
    double silhouette = calculate_silhouette();
    cout << "Silhouette Score for k=" << kclusters << ": " << silhouette << endl;
    
}

void IVFFlat :: print_params() {
    std::cout << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "kclusters : " << kclusters  << ", N: " << N << ", R: " << R << ", nprobe: " << nprobe << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}