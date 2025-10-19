#ifndef SIFT_DATA_HPP
#define SIFT_DATA_HPP

#include <string>
#include <vector>

std::vector<std::vector<float>> read_sift_file(const std::string& filename);
std::vector<std::vector<float>> return_sift_data();
std::vector<std::vector<float>> read_sift_query(const std::string& filename);
std::vector<std::vector<float>> return_sift_queries();

#endif // SIFT_DATA_HPP