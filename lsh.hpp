//lsh.h

#include <iostream>
#include <vector>
using namespace std;

class lsh{
    private:
        std::string input_file, query_file, output_file, type;
        int k, L, N, seed;
        double w, R;
        bool range;

        // std::vector<std::map<uint64_t, std::vector<int>> hashTables;

    public:
        lsh(int seed1, std::string input_file1, std::string query_file1, std::string output_file1, int k1, int L1, float w1, int N1, float R1, std::string type1, bool range1);
        ~lsh();
        void lsh_func(int seed, std::string input_file, std::string query_file, std::string output_file, int k, int L, float w, int N, float R, std::string type, bool range);

         void print_params() {
            std::cout << "input_file: " << input_file << "\n"
                    << "query_file: " << query_file << "\n"
                    << "output_file: " << output_file << "\n"
                    << "k: " << k << ", L: " << L << ", w: " << w << ", N: " << N << ", R: " << R << "\n"
                    << "type: " << type << "\n"
                    << "range: " << std::boolalpha << range << "\n";
        }
};

