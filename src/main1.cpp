//main1.cpp

#include <iostream>
#include "../include/mnist_data.hpp"
#include "../include/sift_data.hpp"

int main(){
    cout << "MNIST:  " << endl;
    vector<float> data_mnist = return_mnist_data();

    cout << endl ;

    cout << "SIFT:  " << endl;
    vector<float> data_sift = return_sift_data();
    // auto mnist_data = read_mnist_file("mnist_data/train-images-idx3-ubyte");
    // cout << "MNIST: " << mnist_data.size() << " × " << mnist_data[0].size() << endl;

    // cout << "\nΠρώτο MNIST διάνυσμα (πρώτα 20 στοιχεία): ";
    // for (int i = 0; i < 784; ++i)
    //     cout << mnist_data[1][i] << " ";
    // cout << endl;
}
