// //main1.cpp

// #include <iostream>
// #include "../include/mnist_data.hpp"
// #include "../include/sift_data.hpp"

// int main(){
//     cout << "MNIST:  " << endl;
//     vector<float> data_mnist = return_mnist_data();

//     cout << endl ;

//     cout << "SIFT:  " << endl;
//     vector<vector<float>> data_sift = return_sift_data();
//     for(int i=0; i<data_sift[0].size(); i++){
//         cout << data_sift[0][i] << " ";
//     }
//     cout << endl;
//     // auto mnist_data = read_mnist_file("mnist_data/train-images-idx3-ubyte");
//     // cout << "MNIST: " << mnist_data.size() << " × " << mnist_data[0].size() << endl;

//     // cout << "\nΠρώτο MNIST διάνυσμα (πρώτα 20 στοιχεία): ";
//     // for (int i = 0; i < 784; ++i)
//     //     cout << mnist_data[1][i] << " ";
//     // cout << endl;
// }


#include <iostream>
#include "../include/mnist_data.hpp"
#include "../include/sift_data.hpp"

using namespace std;

int main(){
    cout << "MNIST: " << endl;
    vector<vector<float>> data_mnist = return_mnist_data();
    
    // Έλεγχος MNIST data
    if(!data_mnist.empty()){
        cout << "MNIST data size: " << data_mnist.size() << " elements" << endl;
        cout << "First 10 MNIST elements: ";
        for(int i = 0; i < data_mnist[0].size(); i++){
            cout << data_mnist[0][i] << " ";
        }
        cout << endl;
    } else {
        cout << "No MNIST data loaded!" << endl;
    }

    cout << endl;

    cout << "SIFT: " << endl;
    vector<vector<float>> data_sift = return_sift_data();
    
    if(!data_sift.empty() && !data_sift[0].empty()){
        cout << "First SIFT vector (first 20 elements): ";
        for(int i = 0; i < data_sift[0].size(); i++){
            cout << data_sift[0][i] << " ";
        }
        cout << endl;
        
        cout << "SIFT data size: " << data_sift.size() << " vectors × " 
             << data_sift[0].size() << " dimensions" << endl;
    } else {
        cout << "No SIFT data loaded!" << endl;
    }
    
    cout << "Program finished successfully!" << endl;
    return 0;
}