/* sift_data.hpp */

#ifndef SIFT_DATA_HPP
#define SIFT_DATA_HPP

#include <string>
#include <vector>

using namespace std;

vector<vector<float>> read_sift_file(const string& filename);
vector<vector<float>> return_sift_data(const string& input_file);
vector<vector<float>> read_sift_query(const string& filename);
vector<vector<float>> return_sift_queries(const string& query_file);

#endif