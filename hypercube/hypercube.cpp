//Hypercube.cpp

// #include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
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

    cout << "LSH Constructed!" << endl;
}

Hypercube :: ~Hypercube(){
    cout << "LSH Deconstructed!" << endl;
}

void Hypercube :: print_params() {
    std::cout << "input_file: " << input_file << "\n"
              << "query_file: " << query_file << "\n"
              << "output_file: " << output_file << "\n"
              << "kproj: " << kproj << ", M: " << M << ", w: " << w << ", N: " << N << ", R: " << R << ", probes: " << probes << "\n"
              << "type: " << type << "\n"
              << "range: " << boolalpha << range << "\n";
}

int Hypercube :: hypercube_dimension(const vector<vector<double>>& dataset, int kproj){
    if(kproj != 14){
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
