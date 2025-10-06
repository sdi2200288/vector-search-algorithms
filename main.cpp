#include <iostream>
#include <cstring>
#include "lsh.hpp"
using namespace std;

/*int main(int argc, char *argv[]){
    std::string input_file, query_file, output_file, type, s;
    int k, L, N;
    double w, R;
    bool range;

    // cout << "You have entered " << argc
    //      << " arguments:" << endl;

    
    int j = 0;
    while(j < argc){
        if(strcmp(argv[j], "-lsh") == 0){
            for (int i = 1; i < argc; i++) {
                std::string arg = argv[i];
                cout << i << "  " << arg <<  " \n";
                if (strcmp(argv[i], "-d") == 0) {input_file = argv[i+1]; cout << input_file <<  " \n";}
                else if (std::string(argv[i]) == "-q") {query_file = argv[i+1]; cout << query_file <<  " \n";}
                else if (arg == "-o") {output_file = argv[i+1];  cout << output_file <<  " \n";}
                else if (arg == "-k") {k = std::stoi(argv[i+1]);  cout << k <<  " \n";}
                else if (arg == "-L") {L = std::stoi(argv[i+1]);  cout << L <<  " \n";}
                else if (arg == "-w") {w = std::stod(argv[i+1]); cout << w << " \n";}
                else if (arg == "-N") {N = std::stoi(argv[i+1]);  cout << N <<  "\n";}
                else if (arg == "-R") {R = std::stod(argv[i+1]); cout << R <<  " \n";}
                else if (arg == "-type") {type = argv[i+1]; cout << type <<  " \n";}
                else if (arg == "-range") {s = argv[i+1]; if(s == "true") range = true; else if(s == "false") range = false; cout<<std::boolalpha<<range<<" \n";}
                
            }
        }
        else if(strcmp(argv[j], "-hypercube") == 0){
            //hypercube func
        }
        

        j++;
    }

     // Δημιουργία αντικειμένου LSH
    lsh LSH(1, input_file, query_file, output_file, k, L, w, N, R, type, range);

    // Έλεγχος τιμών
    LSH.print_params();


    return 0;
}*/

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage example:\n"
             << "./search -d input.dat -q query.dat -k 4 -L 5 -w 4.0 "
             << "-o output.txt -N 1 -R 2000 -type mnist -range true -lsh\n";
        return 1;
    }

    string input_file, query_file, output_file, type;
    int k, L, N , seed;
    double w , R;
    bool range;

    // Flags για ποιον αλγόριθμο θα τρέξει
    bool use_lsh = false;
    bool use_hypercube = false;
    bool use_ivfflat = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "-d") input_file = argv[++i];
        else if (arg == "-q") query_file = argv[++i];
        else if (arg == "-o") output_file = argv[++i];
        else if (arg == "-k") k = stoi(argv[++i]);
        else if (arg == "-L") L = stoi(argv[++i]);
        else if (arg == "-w") w = stod(argv[++i]);
        else if (arg == "-N") N = stoi(argv[++i]);
        else if (arg == "-R") R = stod(argv[++i]);
        else if (arg == "-type") type = argv[++i];
        else if (arg == "-range") {
            string s = argv[++i];
            range = (s == "true");
        }
        //έλεγχος ποιο flag δόθηκε
        else if (arg == "-lsh") use_lsh = true;
        else if (arg == "-hypercube") use_hypercube = true;
        else if (arg == "-ivfflat") use_ivfflat = true;
    }

    if (use_lsh) {
        cout << "\n>>> Running LSH Algorithm...\n";
        lsh LSH(seed, input_file, query_file, output_file, k, L, w, N, R, type, range);
        LSH.print_params();
    }
    else if (use_hypercube) {
        cout << "\n>>> Running Hypercube Algorithm...\n";
    }
    else if (use_ivfflat) {
        cout << "\n>>> Running IVFFlat Algorithm...\n";
    }
    else {
        cerr << "Error: You must specify an algorithm flag (-lsh, -hypercube, or -ivfflat)\n";
    }

    return 0;
}

