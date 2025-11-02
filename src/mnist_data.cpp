#include "../include/mnist_data.hpp"
#include <iostream>
#include <fstream>

using namespace std;

using std::cerr;
using std::cout;
using std::endl;

vector<vector<float>> read_mnist_file(const string& filename, int expected_images){
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "❌ Error: Cannot open file " << filename << endl;
        exit(1);
    }

    int magic = 0, num_images = 0, rows = 0, cols = 0;

    file.read((char*)&magic, 4);
    file.read((char*)&num_images, 4);
    file.read((char*)&rows, 4);
    file.read((char*)&cols, 4);

    // μετατροπή από Big Endian → Little Endian
    magic = __builtin_bswap32(magic);
    num_images = __builtin_bswap32(num_images);
    rows = __builtin_bswap32(rows);
    cols = __builtin_bswap32(cols);

    // έλεγχος magic number για validity
    if (magic != 2051) {
        cerr << "❌ Invalid magic number in MNIST file: " << magic << endl;
        exit(1);
    }

    //  έλεγχος συμφωνίας αναμενόμενων και πραγματικών εικόνων
    if (expected_images > 0 && num_images != expected_images) {
        cerr << "⚠️ Warning: expected " << expected_images 
             << " images but found " << num_images << endl;
    }

    // προ-δέσμευση μνήμης για αποδοτικότητα
    vector<vector<float>> images(num_images, vector<float>(rows * cols));

    for (int i = 0; i < num_images; ++i) {
        vector<unsigned char> buffer(rows * cols);
        file.read((char*)buffer.data(), rows * cols);
        // κανονικοποίηση pixel values στο [0,1]
        for (int j = 0; j < rows * cols; ++j)
            images[i][j] = buffer[j] / 255.0f; // Κανονικοποίηση [0,1]
    }

    cout << "✅ Loaded " << num_images << " MNIST images of dimension " 
         << rows * cols << endl;
    return images;
}



vector<vector<float>> return_mnist_data(const string& input_file) {
    auto mnist_data = read_mnist_file(input_file);
    
    cout << "✅ Created MNIST test data: " << mnist_data.size() << " elements" << endl;
    return mnist_data; // επιστροφή αντιγράφου 
}

vector<vector<float>> read_mnist_query(const string& filename) {
    return read_mnist_file(filename);
}

vector<vector<float>> return_mnist_queries(const string& query_file){
    return read_mnist_query(query_file);
}