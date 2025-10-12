// //mnist_data.hpp

// #ifndef MNSIT_DATA_HPP
// #define MNSIT_DATA_HPP

// #include <string>
// #include <vector>
// using namespace std;

// vector<vector<float>> read_mnist_file(const string& filename, int expected_images);
// vector<float> return_mnist_data();

// #endif


#ifndef MNIST_DATA_HPP
#define MNIST_DATA_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

vector<vector<float>> read_mnist_file(const string& filename, int expected_images = 60000) {
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

    // Μετατροπή από Big Endian → Little Endian
    magic = __builtin_bswap32(magic);
    num_images = __builtin_bswap32(num_images);
    rows = __builtin_bswap32(rows);
    cols = __builtin_bswap32(cols);

    if (magic != 2051) {
        cerr << "❌ Invalid magic number in MNIST file: " << magic << endl;
        exit(1);
    }

    if (expected_images > 0 && num_images != expected_images) {
        cerr << "⚠️ Warning: expected " << expected_images 
             << " images but found " << num_images << endl;
    }

    vector<vector<float>> images(num_images, vector<float>(rows * cols));

    for (int i = 0; i < num_images; ++i) {
        vector<unsigned char> buffer(rows * cols);
        file.read((char*)buffer.data(), rows * cols);
        for (int j = 0; j < rows * cols; ++j)
            images[i][j] = buffer[j] / 255.0f; // Κανονικοποίηση [0,1]
    }

    cout << "✅ Loaded " << num_images << " MNIST images of dimension " 
         << rows * cols << endl;
    return images;
}



vector<vector<float>> return_mnist_data() {
    // Βεβαιώσου ότι αυτή η συνάρτηση ΔΕΝ κάνει double free
    // ή δεν επιστρέφει reference σε τοπικό αντικείμενο
    auto mnist_data = read_mnist_file("../mnist_data/train-images-idx3-ubyte");
    // vector<vector<float>> mnist_data;
    
    
    
    cout << "✅ Created MNIST test data: " << mnist_data.size() << " elements" << endl;
    return mnist_data; // Επιστροφή αντιγράφου - ασφαλές
}

vector<vector<float>> read_mnist_query(const string& filename) {
    return read_mnist_file(filename);
}

vector<vector<float>> return_mnist_queries(){
    return read_mnist_query("../sift_data/sift_query.fvecs");
}

#endif