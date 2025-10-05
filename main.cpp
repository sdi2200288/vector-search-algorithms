#include <iostream>
#include <cstring>
#include "lsh.hpp"
using namespace std;

int main(int argc, char *argv[]){
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
}
