#include "../include/mnist_data.hpp"
#include <iostream>
#include <fstream>

using namespace std;

using std::cerr;
using std::cout;
using std::endl;

vector<vector<float>> read_mnist_file(const string& filename, int expected_images) {
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

//     cout << "✅ Loaded " << num_images << " MNIST images of dimension " 
//          << rows * cols << endl;
//     return images;
// }


// // int main() {
// //     auto mnist_data = read_mnist_file("mnist_data/train-images-idx3-ubyte");
// //     cout << "MNIST: " << mnist_data.size() << " × " << mnist_data[0].size() << endl;

// //     cout << "\nΠρώτο MNIST διάνυσμα (πρώτα 20 στοιχεία): ";
// //     for (int i = 0; i < 784; ++i)
// //         cout << mnist_data[1][i] << " ";
// //     cout << endl;
// // }

// vector<float> return_mnist_data(){
//     auto mnist_data = read_mnist_file("../../mnist_data/train-images-idx3-ubyte");
//     cout << "MNIST: " << mnist_data.size() << " × " << mnist_data[0].size() << endl;

//     // cout << "\nΠρώτο MNIST διάνυσμα (πρώτα 20 στοιχεία): ";
//     // for (int i = 0; i < 784; ++i)
//     //     cout << mnist_data[1][i] << " ";
//     // cout << endl;  

//     //return mnist_data[0];
//     // for(int i=0; i<mnist_data.size(); i++){
//         for(int j=0; j<mnist_data[0].size(); ++j){
//             cout << mnist_data[0][j] << " ";
//         }
//         cout << endl;
    
//     // }
//     // for(int i=0; i<784; i++){
//     //     return mnist_data[0][i];
//     // }
// }
