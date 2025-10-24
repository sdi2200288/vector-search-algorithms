//Hypercube.cpp

// #include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <climits>
#include <chrono>

#include "hypercube.hpp"

// using namespace std;

Hypercube :: Hypercube(int seed1, string input_file1, string query_file1, string output_file1, int kproj1, int M1, double w1, int N1, double R1, int probes1, string type1, bool range1){
    seed = seed1;
    input_file = input_file1;
    query_file = query_file1;
    output_file = output_file1;
    kproj = kproj1;
    M = M1;
    w = w1;
    N = N1;
    R = R1;
    probes = probes1;
    type = type1;
    range = range1;

    cout << "Hypercube Constructed!" << endl;
}

Hypercube :: ~Hypercube(){
    cout << "Hypercube Deconstructed!" << endl;
}

void Hypercube :: print_params() {
    cout << "input_file: " << input_file << "\n"
         << "query_file: " << query_file << "\n"
         << "output_file: " << output_file << "\n"
         << "kproj: " << kproj << ", M: " << M << ", w: " << w << ", N: " << N << ", R: " << R << ", probes: " << probes << "\n"
         << "type: " << type << "\n"
         << "range: " << boolalpha << range << "\n";
}

int Hypercube :: hypercube_dimension(const vector<vector<double>>& dataset, int kproj){
    if(kproj > 0){
        return kproj;
    }

    int n = dataset.size();
    // cout << "dataset_size: " << dataset.size() << endl;
    int log_n = static_cast<int>(floor(log2(n)));

    random_device rd;                       //δημιουργεί έναν τυχαίο αριθμοπαραγωγό hardware-based
    mt19937 gen(rd());                      //δημιουργεί έναν Mersenne Twister generator
    uniform_int_distribution<> dist(1, 3);  //φτιάχνει μια κατανομή που παράγει ακέραιους αριθμούς στο διάστημα [1, 3] με ίση πιθανότητα
    int rand_offset = dist(gen);            //παίρνουμε έναν πραγματικό τυχαίο αριθμό.
    int d_prime = log_n - rand_offset;      //υπολογίζει μια νέα τιμή d_prime

    // cout << "rand_offset : " << rand_offset << " d_prime : " << d_prime << endl; 

    return max(d_prime, 1);                 //επιστρέφει τη μέγιστη τιμή μεταξύ d_prime και 1 για την διασταση
}

void Hypercube :: initialize_LSH_func(const vector<vector<double>>& dataset){
    //αρχικοποίηση random number generator με το seed
    mt19937 gen(seed);
    normal_distribution<double> normal_dist(0.0, 1.0);
    uniform_real_distribution<double> uniform_dist(0.0, w);

    int d_prime = hypercube_dimension(dataset, kproj);  //υπολογισμός της διάστασης του hypercube
    int dimension = dataset[0].size();                  //διασταση των διανυσμάτων

    v.clear();  //καθαρισμός από τις προηγούμενες τιμές
    t.clear();

    cout << "LSH funcs: " << d_prime << "  dimension: " << dimension << endl;

    //δημιουργία LSH funcs για κάθε διάσταση του hypercube
    for(int i=0; i<d_prime; i++){
        vector<double> v_1;
        for(int j=0; j<dimension; j++){
            v_1.push_back(normal_dist(gen));
        }
        v.push_back(v_1);
        t.push_back(uniform_dist(gen));
    }
} 

string Hypercube :: project_to_hypercube(const vector<vector<double>>& dataset, const vector<double>& point){
    int d_prime = hypercube_dimension(dataset, kproj);  //υπολογισμός της διάστασης του hypercube
    int dimension = dataset[0].size();                  //διασταση των διανυσμάτων

    string vertex;
    for(int i=0; i<d_prime; i++){
        //LSH h_i(p) = floor((v_i·p + t_i) / w)
        double dot_product = 0.0;
        for(int j=0; j<dimension; j++){
            dot_product += v[i][j]*point[j];
        }
        int numerator = static_cast<int>(floor((dot_product + t[i]) / w));
        
        //f_i mapping to {0,1} uniformly (mod 2)
        char bit = (numerator % 2 == 0) ? '0' : '1';

        //προσθήκη στη δυαδική συμβολοσειρά
        vertex += bit;
    }
    return vertex;
}

void Hypercube :: build_index(const vector<vector<double>>& dataset){        //κατασκευή ευρετηρίου υπερκύβου
    initialize_LSH_func(dataset);   //αρχικοποίηση των LSH funcs
    h_map.clear();  
    
    this->dataset_reference = dataset;

    cout << "hypercube index size: " << dataset.size() << endl;

    //αποθήκευση του id
    string vertex;
    for(size_t i=0; i<dataset.size(); i++){     //με int χτυπουσε warning
        vertex = project_to_hypercube(dataset, dataset[i]);
        h_map[vertex].push_back(i);
    }

    cout << "h_map size: " << h_map.size() << endl;

    //στατιστικά κατασκευής για debugging & εκφώνηση
    cout << "\n=== Hypercube Index Statistics ===" << endl;
    cout << "Total vertices used: " << h_map.size() << " out of " << (1 << kproj) << " possible" << endl;

    size_t total_points = 0;
    size_t max_vertex_points = 0;
    size_t min_vertex_points = dataset.size();

    for(const auto& i : h_map){
        size_t vertex_points = i.second.size();
        total_points += vertex_points;
        max_vertex_points = max(max_vertex_points, vertex_points);
        min_vertex_points = min(min_vertex_points, vertex_points);
    }

    cout << "Total points stored: " << total_points << "/" << dataset.size() << endl;
    cout << "Max points per vertex: " << max_vertex_points << endl;
    cout << "Min points per vertex: " << min_vertex_points << endl;
    cout << "Average points per vertex: " << (double)total_points / h_map.size() << endl;
    cout << "==================================" << endl;
}

vector<string> Hypercube :: vertices_at_hamming_dist(const string& vertex, int target_dist, int max_vertices){  //υπολογισμός hamming distance
    vector<string> res;
    int n = vertex.length();
    
    if(target_dist == 0){       //αν η απόσταση είναι 0 τότε επιστρέφουμε μόνο τη αρχική κορυφή 
        res.push_back(vertex);
        return res;
    }
    
    if(target_dist > n){        //αν η απόσταση είναι μεγαλύτερη απο το μήκος τότε επιστρέφουμε κενή λίστα
        return res;
    }
    
    vector<int> positions(n);
    for(int i = 0; i < n; i++){
        positions[i] = i;
    }
    
    vector<bool> mask(n, false);
    for(int i = 0; i < target_dist; i++){
        mask[i] = true;
    }
    
    int count = 0;
    do{    //δημιουργία όλων των συνδυασμών target_dist θέσεων από n
        string new_vertex = vertex;     //aντιγραφή αρχικής κορυφής
        
        for(int i = 0; i < n; i++){
            if(mask[i]){
                new_vertex[i] = (new_vertex[i] == '0') ? '1' : '0';
            }
        }
        
        res.push_back(new_vertex);
        count++;

        if(count >= max_vertices) break;
        
    }while(prev_permutation(mask.begin(), mask.end()));     //επόμενος συνδυασμός
    
    return res;
}

double Hypercube::euclidean_distance(const vector<double>& v1, const vector<double>& v2){
    if(v1.size() != v2.size()){     //έλεγχος οτι είναι ιδιας διάστασης
        throw invalid_argument("Vectors must have same dimensions");
    }

    double sum_squares = 0.0;

    for(size_t i = 0; i < v1.size(); i++){
        double diff = v1[i] - v2[i];
        sum_squares += diff * diff;
        
        if(sum_squares > 1e10){  //Threshold, exit για πολύ μεγάλες αποστάσεις 
            break;
        }
    }
    
    return sqrt(sum_squares);
}

vector<pair<int, double>> Hypercube :: approximate_nearest_neighbors(const vector<double>& query){
    vector<pair<int, double>> candidates;
    int points_checked = 0;
    int vertices_probed = 0;
    
    string query_vertex = project_to_hypercube(dataset_reference, query);   //προβολη query σε υπερκυβο
    
    for(int hamming_dist = 0; hamming_dist <= kproj; hamming_dist++){       //αναζήτηση με αυξανομένη hamming απόσταση 
        auto vertices_at_dist = vertices_at_hamming_dist(query_vertex, hamming_dist, probes - vertices_probed);
        
        for(const auto& vertex : vertices_at_dist){
            if(vertices_probed >= probes) break;
            
            if(h_map.find(vertex) != h_map.end()){
                for(int image_id : h_map[vertex]){
                    if(points_checked >= M) break;
                    
                    double dist = euclidean_distance(dataset_reference[image_id], query);
                    candidates.push_back({image_id, dist});
                    points_checked++;
                }
                vertices_probed++;
            }
            if(points_checked >= M) break;
        }
        if(points_checked >= M || vertices_probed >= probes) break;
    }
    
    sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) { return a.second < b.second; });   //ταξινόμηση και επιστροφη κοντινότερων γειτόνων
    
    if(candidates.size() > N){
        candidates.resize(N);
    }
    
    return candidates;
}

vector<pair<int, double>> Hypercube::exact_nearest_neighbors(const vector<double>& query){
    vector<pair<int, double>> all_distances;
    
    for(int i = 0; i < dataset_reference.size(); i++){      //υπολογισμός απόστασης απο όλα τα σημεία
        double dist = euclidean_distance(dataset_reference[i], query);
        all_distances.push_back({i, dist});
    }
    
    sort(all_distances.begin(), all_distances.end(), [](const auto& a, const auto& b) { return a.second < b.second; });     //ταξινόμηση κατά αύξουσα απόσταση
    
    if(all_distances.size() > N){   //επιστρτοφή των n πρωτων πλησιέστερων
        all_distances.resize(N);
    }
    
    return all_distances;
}

vector<int> Hypercube :: approximate_range_search(const vector<double>& query){   //range search
    vector<int> results;
    int vertices_probed = 0;
    int points_checked = 0;     //μετρητής για το M, ώστε ο αλγόριθμος να σταματα αν ελεγχθουν Μ υποψήφια σημεία
    
    string query_vertex = project_to_hypercube(dataset_reference, query);
    
    int dim = hypercube_dimension(dataset_reference, kproj);
    for(int i = 0; i <= dim; i++){
        auto vertices_at_dist = vertices_at_hamming_dist(query_vertex, i, probes - vertices_probed);
        
        for(const auto& vertex : vertices_at_dist){
            if(vertices_probed >= probes) break;
            if(points_checked >= M) break;  //ελεγχος για M
            
            if(h_map.find(vertex) != h_map.end()){
                for(int image_id : h_map[vertex]){
                    if(points_checked >= M) break;  //ελεγχος για M

                    double dist = euclidean_distance(dataset_reference[image_id], query);
                    if(dist <= R){
                        results.push_back(image_id);
                    }
                    points_checked++;  //αύξηση μετρητή
                }
                vertices_probed++;
            }
        }
        if(vertices_probed >= probes  || points_checked >= M) break;
    }
    
    return results;
}

vector<int> Hypercube :: exact_range_search(const vector<double>& query){
    vector<int> results;
    
    for(size_t i = 0; i < dataset_reference.size(); i++){
        double dist = euclidean_distance(dataset_reference[i], query);
        if(dist <= R) results.push_back(i);
    }
    
    return results;
}

void Hypercube :: Queries(const vector<vector<double>>& queries,const vector<vector<double>>& dataset){
    cout<<"Calculate Queries"<<endl;

    ofstream out(output_file);
    if(!out.is_open()){
        cerr << "Error: Cannot open output file " << output_file << endl;
        return;
    }
    
    out<<"Hypercube\n\n";

    double total_Average_factor = 0.0, total_recall=0.0;
    double total_atime = 0.0, total_etime = 0.0;
    int valid_q = 0;

    auto start_all = chrono::high_resolution_clock::now();
    
    for(int i = 0; i < queries.size(); i++){
        const auto& q = queries[i];

        //Προσεγγιστική αναζήτηση
        auto start = chrono::high_resolution_clock::now();
        auto aNN = approximate_nearest_neighbors(q);    // vector<pair<int,double>>
        auto end = chrono::high_resolution_clock::now();
        auto atime = chrono::duration<double>(end-start).count();
        
        //Ακριβής αναζήτηση
        start = chrono::high_resolution_clock::now();
        vector<pair<int,double>> eNN = exact_nearest_neighbors(q);    //vector<pair<int,double>>
        end = chrono::high_resolution_clock::now();
        auto etime = chrono::duration<double>(end-start).count();
        
        valid_q++;
        total_atime += atime;
        total_etime += etime;
   
        out << "Query: " << i <<endl;
        if(!aNN.empty() && !eNN.empty()) {
            
            for(int neighbor = 0; neighbor < aNN.size(); neighbor++){
                int data_idx =  aNN[neighbor].first;
                
                out << "Nearest neighbor-" << neighbor + 1 << ": " << data_idx << endl ;
                out << "distanceApproximate: " << aNN[neighbor].second << endl;
        
                if(neighbor < eNN.size()){
                    out<<"distanceTrue: " << eNN[neighbor].second << endl;
                }
            }
            
            double Average_factor = aNN[0].second / eNN[0].second;
            total_Average_factor += Average_factor;
            
            int recall_hits = 0;
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
            
            //Προσεγγιστική αναζήτηση
            auto start = chrono::high_resolution_clock::now();
            auto aRN = approximate_range_search(q);    // vector<pair<int,double>>
            auto end = chrono::high_resolution_clock::now();
            auto atime = chrono::duration<double>(end-start).count();
            
            //Ακριβής αναζήτηση
            start = chrono::high_resolution_clock::now();
            auto eRN = exact_range_search(q);    //vector<pair<int,double>>
            end = chrono::high_resolution_clock::now();
            auto etime = chrono::duration<double>(end-start).count();

            out << "R-near neigbours" << endl;
            if(!eRN.empty() && !aRN.empty()){
                for(int id : aRN){
                    out << id << endl;
                }

                for(int id : eRN){
                    out << id << endl;
                }
            }            

            double total_time_all = chrono::duration<double>(chrono::high_resolution_clock::now()-start_all).count();
            double qps = (valid_q > 0) ? valid_q / total_time_all : 0.0;

            out<< "Average AF: " << Average_factor << endl;
            out<< "Recall@N: " << recall <<endl;
            out << "QPS: " << qps << endl;
            out << "tApproximateAverage: " << atime << endl;
            out << "tTrueAverage: " << etime << endl;
            out << "-------------------" << endl;
        }     
    }

    out.close();
    cout<<"Complete Queries"<<endl;

   
}

void Hypercube :: hypercube_func(const vector<vector<double>>& dataset,const vector<vector<double>>& queries){

    cout<<"LSH Algorithm" <<endl;

    cout << "Dataset size: " << dataset.size() << " vectors" << endl;
    cout << "Vector dimension: " << dataset[0].size() << endl;

    Queries(queries,dataset);

}