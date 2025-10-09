// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <string>
// #include "sift.hpp"

// using namespace std;

// vector<vector<float>> read_mnist_file(const string& filename, int expected_images) {
//     ifstream file(filename, ios::binary);
//     if (!file.is_open()) {
//         cerr << "❌ Error: Cannot open file " << filename << endl;
//         exit(1);
//     }

//     int magic = 0, num_images = 0, rows = 0, cols = 0;
//     file.read((char*)&magic, 4);
//     magic = __builtin_bswap32(magic); // MNIST = big endian
//     if (magic != 2051) {
//         cerr << "❌ Invalid magic number in MNIST file: " << magic << endl;
//         exit(1);
//     }

//     file.read((char*)&num_images, 4);
//     file.read((char*)&rows, 4);
//     file.read((char*)&cols, 4);
//     num_images = __builtin_bswap32(num_images);
//     rows = __builtin_bswap32(rows);
//     cols = __builtin_bswap32(cols);

//     if (expected_images > 0 && num_images != expected_images)
//         cerr << "⚠️ Warning: expected " << expected_images << " images but found " << num_images << endl;

//     vector<vector<float>> images(num_images, vector<float>(rows * cols));

//     for (int i = 0; i < num_images; ++i) {
//         vector<unsigned char> buffer(rows * cols);
//         file.read((char*)buffer.data(), rows * cols);
//         for (int j = 0; j < rows * cols; ++j)
//             images[i][j] = buffer[j] / 255.0f; // κανονικοποίηση [0,1]
//     }

//     cout << "✅ Loaded " << num_images << " MNIST images of dimension " << rows * cols << endl;
//     return images;
// }


// vector<vector<float>> read_sift_file(const string& filename) {
//     ifstream file(filename, ios::binary);
//     if (!file.is_open()) {
//         cerr << "❌ Error: Cannot open file " << filename << endl;
//         exit(1);
//     }

//     vector<vector<float>> data;
//     int d; // dimension (should be 128)

//     while (file.read((char*)&d, sizeof(int))) {
//         if (d != 128) {
//             cerr << "⚠️ Unexpected dimension value: " << d << endl;
//             break;
//         }

//         vector<float> vec(d);
//         file.read((char*)vec.data(), d * sizeof(float));
//         if (!file) break;

//         data.push_back(vec);
//     }

//     cout << "✅ Loaded " << data.size() << " vectors of dimension " 
//          << (data.empty() ? 0 : data[0].size()) << endl;

//     return data;
// }

// int main() {
//     string filename = "../../sift/sift_base.fvecs";  // <-- το δικό σου αρχείο
//     auto data = read_sift_file(filename);

//     string filename1 = "../../sift/sift_query.fvecs";
//     auto data1 = read_sift_file(filename1);

//     string filename2 = "train-images-idx3-ubyte";
//     auto data2 = read_mnist_file(filename2, 60000);

//     // cout << "First vector (first 10 coords): ";
//     // for(int j =0; j <5; j++){
//     // for (int i = 0; i < 10; ++i)
//     //     cout << data[j][i] << " ";
//     // cout << endl;}

//     //  cout << "First query (first 10 coords): ";
//     // for(int j=0; j<5; j++){
//     //  for (int i = 0; i < 10; ++i)
//     //     cout << data1[j][i] << " ";
//     // cout << endl;}

//     // cout << "First query (first 10 coords): ";
//     // for(int j=0; j<5; j++){
//     //  for (int i = 0; i < 10; ++i)
//     //     cout << data2[j][i] << " ";
//     // cout << endl;}
// }


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <zlib.h>
using namespace std;

// =============================================================
// 📘 1. SIFT Loader (fvecs format - Little Endian, float32)
// =============================================================
vector<vector<float>> read_sift_file(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "❌ Error: Cannot open file " << filename << endl;
        exit(1);
    }

    vector<vector<float>> data;
    int d = 0; // dimension (should be 128)

    while (file.read((char*)&d, sizeof(int))) {
        if (d != 128) {
            cerr << "⚠️ Unexpected dimension value: " << d << " (expected 128)" << endl;
            break;
        }

        vector<float> vec(d);
        file.read((char*)vec.data(), d * sizeof(float));
        if (!file) break;
        data.push_back(vec);
    }

    cout << "✅ Loaded " << data.size() << " SIFT vectors of dimension "
         << (data.empty() ? 0 : data[0].size()) << endl;
    return data;
}

// =============================================================
// 🖼️ 2. MNIST Loader (idx3-ubyte format - Big Endian, uint8)
// =============================================================
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

// =============================================================
// 🔁 3. Wrapper για SIFT ή MNIST
// =============================================================
vector<vector<float>> load_dataset(const string& filename, const string& type) {
    if (type == "sift")
        return read_sift_file(filename);
    else if (type == "mnist")
        return read_mnist_file(filename, 60000);
    else {
        cerr << "❌ Unknown dataset type: " << type << endl;
        exit(1);
    }
}

// =============================================================
// 🧩 4. Παράδειγμα Χρήσης (main)
// =============================================================
int main() {
    cout << "=== Φόρτωση SIFT ===" << endl;
    auto sift_data = load_dataset("../sift_data/sift_base.fvecs", "sift");

    cout << "=== Φόρτωση SIFT Queries ===" << endl;
    auto sift_queries = load_dataset("..//sift_query.fvecs", "sift");

    cout << "=== Φόρτωση MNIST ===" << endl;
    auto mnist_data = load_dataset("train-images-idx3-ubyte", "mnist");

    cout << "\n📊 Περίληψη:\n";
    cout << "SIFT base: " << sift_data.size() << " × " << sift_data[0].size() << endl;
    cout << "SIFT queries: " << sift_queries.size() << " × " << sift_queries[0].size() << endl;
    cout << "MNIST: " << mnist_data.size() << " × " << mnist_data[0].size() << endl;

    // Παράδειγμα πρόσβασης στα πρώτα διανύσματα
    cout << "\nΠρώτο MNIST διάνυσμα (πρώτα 20 στοιχεία): ";
    for (int i = 0; i < 784; ++i)
        cout << mnist_data[1][i] << " ";
    cout << endl;

    cout << "\nΠρώτο SIFT διάνυσμα (πρώτα 10 στοιχεία): ";
    for (int i = 0; i < 128; ++i)
        cout << sift_data[0][i] << " ";
    cout << endl;

    return 0;
}
