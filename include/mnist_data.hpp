#ifndef MNIST_DATA_HPP
#define MNIST_DATA_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

vector<vector<float>> read_mnist_file(const string& filename, int expected_images = 60000);
vector<vector<float>> return_mnist_data(const string& input_file);
vector<vector<float>> read_mnist_query(const string& filename);
vector<vector<float>> return_mnist_queries(const string& query_file);

#endif