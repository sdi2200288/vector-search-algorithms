#!/usr/bin/env python3

import gzip
import numpy as np
import os
import struct
from urllib.request import urlretrieve


def load_data(src, num_samples):
    print(f"Downloading {src}")
    gzfname, _ = urlretrieve(src, os.path.basename(src))
    print(f"Saved as {gzfname}")
    try:
        with gzip.open(gzfname, "rb") as gz:
            # Διαβάζει το magic number
            n = struct.unpack("I", gz.read(4))
            if n[0] != 0x3080000:
                raise Exception("Invalid file: unexpected magic number.")
            # Διαβάζει το πλήθος των εικόνων
            n = struct.unpack(">I", gz.read(4))[0]
            if n != num_samples:
                raise Exception(f"Invalid file: expected {num_samples} entries.")
            crow = struct.unpack(">I", gz.read(4))[0]
            ccol = struct.unpack(">I", gz.read(4))[0]
            if crow != 28 or ccol != 28:
                raise Exception("Invalid file: expected 28 rows/cols per image.")
            # Διαβάζει τα δεδομένα των pixels
            res = np.frombuffer(gz.read(num_samples * crow * ccol), dtype=np.uint8)
    except Exception as e:
        print("Error reading data:", e)
        raise
    return res.reshape((num_samples, crow, ccol)) / 256


def load_labels(src, num_samples):
    print(f"Downloading {src}")
    gzfname, _ = urlretrieve(src, os.path.basename(src))
    print(f"Saved as {gzfname}")
    try:
        with gzip.open(gzfname, "rb") as gz:
            # Διαβάζει το magic number
            n = struct.unpack("I", gz.read(4))
            if n[0] != 0x1080000:
                raise Exception("Invalid file: unexpected magic number.")
            # Διαβάζει το πλήθος των labels
            n = struct.unpack(">I", gz.read(4))
            if n[0] != num_samples:
                raise Exception(f"Invalid file: expected {num_samples} rows.")
            # Διαβάζει τα labels
            res = np.frombuffer(gz.read(num_samples), dtype=np.uint8)
    except Exception as e:
        print("Error reading labels:", e)
        raise
    return res.reshape((num_samples,))


def try_download(data_source, label_source, num_samples):
    data = load_data(data_source, num_samples)
    labels = load_labels(label_source, num_samples)
    return data, labels


def get_mnist():
    # Κατεβάζει τα δεδομένα MNIST από GitHub mirror
    server = "https://raw.githubusercontent.com/fgnt/mnist/master"

    # URLs για train
    url_train_image = f"{server}/train-images-idx3-ubyte.gz"
    url_train_labels = f"{server}/train-labels-idx1-ubyte.gz"
    num_train_samples = 60000

    print("\n=== Downloading TRAIN data ===")
    train_features, train_labels = try_download(url_train_image, url_train_labels, num_train_samples)

    # URLs για test
    url_test_image = f"{server}/t10k-images-idx3-ubyte.gz"
    url_test_labels = f"{server}/t10k-labels-idx1-ubyte.gz"
    num_test_samples = 10000

    print("\n=== Downloading TEST data ===")
    test_features, test_labels = try_download(url_test_image, url_test_labels, num_test_samples)

    return train_features, train_labels, test_features, test_labels


if __name__ == "__main__":
    print(">>> Starting MNIST download...\n")
    train_x, train_y, test_x, test_y = get_mnist()
    print("\n>>> Done!")
    print("Train set:", train_x.shape, train_y.shape)
    print("Test set:", test_x.shape, test_y.shape)
