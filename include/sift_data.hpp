//sift_data.hpp

#ifndef SIFT_DATA_HPP
#define SIFT_DATA_HPP

#include <string>
#include <vector>
using namespace std;

vector<vector<float>> read_sift_file(const string& filename);
vector<float> return_sift_data();

#endif