//κανει download τα αρχεια του mnist
//τρεχει με το: g++ mnist.cpp -o mnist -lcurl -lz

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <zlib.h>
#include <curl/curl.h>

// ---------------- Struct για MNIST ----------------
struct MNISTData {
    std::vector<std::vector<unsigned char>> train_images;
    std::vector<unsigned char> train_labels;
    std::vector<std::vector<unsigned char>> test_images;
    std::vector<unsigned char> test_labels;
};

// ---------------- Helpers ----------------
size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    return fwrite(ptr, size, nmemb, stream);
}

void download_file(const std::string& url, const std::string& out_filename) {
    CURL* curl = curl_easy_init();
    if (!curl) throw std::runtime_error("curl init failed");

    FILE* fp = fopen(out_filename.c_str(), "wb");
    if (!fp) throw std::runtime_error("cannot open file for writing");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    std::cout << "Downloading " << url << std::endl;
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
        throw std::runtime_error("Download failed: " + std::string(curl_easy_strerror(res)));

    curl_easy_cleanup(curl);
    fclose(fp);
    std::cout << "Saved as " << out_filename << std::endl;
}

std::vector<unsigned char> decompress_gzip(const std::string& filename) {
    gzFile gz = gzopen(filename.c_str(), "rb");
    if (!gz) throw std::runtime_error("Failed to open gzip file: " + filename);

    std::vector<unsigned char> buffer;
    unsigned char temp[4096];
    int bytes_read;
    while ((bytes_read = gzread(gz, temp, sizeof(temp))) > 0)
        buffer.insert(buffer.end(), temp, temp + bytes_read);

    gzclose(gz);
    return buffer;
}

std::vector<std::vector<unsigned char>> load_images(const std::string& filename, int num_samples) {
    std::vector<unsigned char> bytes = decompress_gzip(filename);

    int magic = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    if (magic != 2051) throw std::runtime_error("Invalid magic number for images");

    int count = (bytes[4] << 24) | (bytes[5] << 16) | (bytes[6] << 8) | bytes[7];
    int rows = (bytes[8] << 24) | (bytes[9] << 16) | (bytes[10] << 8) | bytes[11];
    int cols = (bytes[12] << 24) | (bytes[13] << 16) | (bytes[14] << 8) | bytes[15];

    if (count != num_samples || rows != 28 || cols != 28)
        throw std::runtime_error("Unexpected image dimensions");

    std::vector<std::vector<unsigned char>> images(num_samples, std::vector<unsigned char>(rows * cols));
    size_t offset = 16;
    for (int i = 0; i < num_samples; ++i) {
        std::copy(bytes.begin() + offset, bytes.begin() + offset + rows * cols, images[i].begin());
        offset += rows * cols;
    }

    return images;
}

std::vector<unsigned char> load_labels(const std::string& filename, int num_samples) {
    std::vector<unsigned char> bytes = decompress_gzip(filename);

    int magic = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    if (magic != 2049) throw std::runtime_error("Invalid magic number for labels");

    int count = (bytes[4] << 24) | (bytes[5] << 16) | (bytes[6] << 8) | bytes[7];
    if (count != num_samples) throw std::runtime_error("Unexpected number of labels");

    return std::vector<unsigned char>(bytes.begin() + 8, bytes.begin() + 8 + num_samples);
}

// ---------------- get_mnist ----------------
MNISTData get_mnist() {
    MNISTData data;
    std::string server = "https://raw.githubusercontent.com/fgnt/mnist/master";

    std::string train_img_url = server + "/train-images-idx3-ubyte.gz";
    std::string train_lbl_url = server + "/train-labels-idx1-ubyte.gz";
    std::string test_img_url = server + "/t10k-images-idx3-ubyte.gz";
    std::string test_lbl_url = server + "/t10k-labels-idx1-ubyte.gz";

    download_file(train_img_url, "train-images-idx3-ubyte.gz");
    download_file(train_lbl_url, "train-labels-idx1-ubyte.gz");
    download_file(test_img_url, "t10k-images-idx3-ubyte.gz");
    download_file(test_lbl_url, "t10k-labels-idx1-ubyte.gz");

    std::cout << "\nReading training data...\n";
    data.train_images = load_images("train-images-idx3-ubyte.gz", 60000);
    data.train_labels = load_labels("train-labels-idx1-ubyte.gz", 60000);

    std::cout << "Reading test data...\n";
    data.test_images = load_images("t10k-images-idx3-ubyte.gz", 10000);
    data.test_labels = load_labels("t10k-labels-idx1-ubyte.gz", 10000);

    return data;
}

// ---------------- main ----------------
int main() {
    try {
        std::cout << ">>> Starting MNIST download...\n";
        MNISTData mnist = get_mnist();

        std::cout << "\n>>> Done!\n";
        std::cout << "Train set: " << mnist.train_images.size() << " images, "
                  << mnist.train_labels.size() << " labels\n";
        std::cout << "Test set: " << mnist.test_images.size() << " images, "
                  << mnist.test_labels.size() << " labels\n";

        // Παράδειγμα πρόσβασης pixel
        std::cout << "Pixel (0,0) πρώτης εικόνας: " 
                  << (int)mnist.train_images[5][5] << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
