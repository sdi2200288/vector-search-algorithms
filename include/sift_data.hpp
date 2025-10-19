#ifndef SIFT_DATA_HPP
#define SIFT_DATA_HPP

#include <string>
#include <vector>
#include <iostream>

using namespace std;

vector<vector<float>> read_sift_file(const string& filename);
vector<vector<float>> return_sift_data();
vector<vector<float>> read_sift_query(const string& filename);
vector<vector<float>> return_sift_queries();

#endif // SIFT_DATA_HPP