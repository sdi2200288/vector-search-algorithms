//mnist_data.hpp

#ifndef MNSIT_DATA_HPP
#define MNSIT_DATA_HPP

#include <string>
#include <vector>
using namespace std;

vector<vector<float>> read_mnist_file(const string& filename, int expected_images);
vector<float> return_mnist_data();

#endif