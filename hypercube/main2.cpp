#include <unistd.h>
// #include <iostream>
#include <cstring>

#include "hypercube.hpp"


// using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage example:\n"
             << "./search -d input.dat -q query.dat -k 4 -L 5 -w 4.0 "
             << "-o output.txt -N 1 -R 2000 -type mnist -range true -lsh\n";
        return 1;
    }

    // Flags για ποιον αλγόριθμο θα τρέξει
    bool use_hypercube = false;
    bool use_ivfflat = false;

    for(int i=0; i<argc; i++){
        string arg = argv[i];        
        if(arg == "-hypercube"){
            use_hypercube = true;
            break;
        }
        else if(arg == "-ivfflat"){
            use_ivfflat = true;
            break;
        }
    }

    string input_file, query_file, output_file, type;
    int kproj = 14, M = 10, N = 1, probes = 2, seed = 1;
    double w = 4.0, R;
    bool range = false, R_flag = false;

    if(use_hypercube){
        cout << "\n>>> Running Hypercube Algorithm...\n";
        for(int i = 1; i<argc; i++){
            string arg = argv[i];
            if(arg == "-d") input_file = argv[++i];
            else if(arg == "-q") query_file = argv[++i];
            else if(arg == "-kproj") kproj = stoi(argv[++i]);
            else if(arg == "-w") w = stod(argv[++i]);
            else if(arg == "-M") M = stoi(argv[++i]);
            else if(arg == "-probes") probes = stoi(argv[++i]); 
            else if(arg == "-o") output_file = argv[++i];
            else if(arg == "-N") N = stoi(argv[++i]);
            else if(arg == "-R"){ R_flag = true; R = stod(argv[++i]);}
            else if(arg == "-type") type = argv[++i];
            else if (arg == "-range") {
                string s = argv[++i];
                range = (s == "true");
            }
        }
        cout << "R_flag: " << R_flag << endl;
        if(!R_flag){
            if(type == "mnist") R = 2000.0;
            else if(type == "sift") R = 2.0;
        }

        Hypercube Hypercube(seed, input_file, query_file, output_file, kproj, M, w, N, R, probes, type, range);
        Hypercube.print_params();
        cout << endl;

        vector<vector<double>> test_dataset = {{1.0, 2.0, 0.5, 1.5, 2.5, 0.1, 1.8, 2.2, 0.8, 1.2}, {2.0, 3.0, 1.5, 2.5, 3.5, 1.1, 2.8, 3.2, 1.8, 2.2}};
        vector<double> test_point = {1.0, 2.0, 0.5, 1.5, 2.5, 0.1, 1.8, 2.2, 0.8, 1.2};

        int dim = Hypercube.hypercube_dimension(test_dataset, kproj);  
        cout << "dim : " << dim << endl; 

        //δημιουργία index
        Hypercube.build_index(test_dataset);

        //test για προβολης για ενα σημειο
        string vertex = Hypercube.project_to_hypercube(test_dataset, test_point);
        cout << "First point projects to vertex: " << vertex << endl;

        //test υπολογισμού διάστασης
        int dim1 = Hypercube.hypercube_dimension(test_dataset, kproj);
        cout << "Computed hypercube dimension: " << dim1 << endl;

        //δοκιμή ότι δουλεύει σωστά:
        cout << "Index built successfully!" << endl;
        cout << "Stored points in hypercube: " << Hypercube.get_h_map().size() << " vertices" << endl;
        cout << "Dataset reference stored: " << Hypercube.get_dataset_reference().size() << " points" << endl;

        //έλεγχος ότι τα δεδομένα είναι προσβάσιμα:
        if (!Hypercube.get_dataset_reference().empty()) {
            cout << "First point in dataset: ";
            for (auto val : Hypercube.get_dataset_reference()[0]) {
                cout << val << " ";
            }
            cout << endl;
        }
    }
    else if (use_ivfflat) {
        cout << "\n>>> Running IVFFlat Algorithm...\n";
    }
    else {
        cerr << "Error: You must specify an algorithm flag (-lsh, -hypercube, or -ivfflat)\n";
    }

    return 0;
}

