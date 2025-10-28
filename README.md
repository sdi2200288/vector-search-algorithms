# Ανάπτυξη Λογισμικού για Αλγοριθμικά Προβλήματα - 1η Προγραμματιστική Εργασία - Αναζήτηση διανυσμάτων στη C/C++

## Περιγρφή 
Το project έχει υλοποιηθεί σε C++, η πειαματική ανάλυση σε python και υπάρχει ένα bash αρχείο το οποίο 
χρησιμοποιείται για την πειραματική ανάλυση.

**Υλοποιημένοι Αλγόριθμοι**
1. LSH (Locality Sensitive Hashing)
2. Hypercube (Τυχαία προβολή σε Υπερκύβο)
3. IVFFlat (Inverted File Flat)
4. IVFPQ (Inverted File Product Quantization)

## Κατάλογος Αρχείων
### Αρχεία πηγαίου κώδικα -> **src/**
1. **lsh.cpp**         : υλοποίηση LSH αλγορίθμου
2. **hypercube.cpp**   : υλοποίηση Hypercube αλγορίθμου
3. **IVFFlat.cpp**     : υλοποίηση IVFFlat αλγορίθμου
4. **ivfpq.cpp**       : υλοποίηση IVFPQ αλγορίθμου
5. **ivfpq_index.cpp** : βοηθητική δομή για IVFPQ
6. **k_means.cpp**     : αλγόριθμος k-means clustering
7. **mnist_data.cpp**  : διαχείριση MNIST dataset
8. **sift_data.cpp**   : διαχείριση SIFT dataset

### Αρχεία επικεφαλίδας -> **include/**
1. **hypercube.hpp**
2. **IVFFlat.hpp**
3. **ivfpq_index.hpp**
4. **ivfpq.hpp**
5. **k_means.hpp**
6. **lsh.hpp**
7. **mnist_data.hpp**
8. **sift_data.hpp**
