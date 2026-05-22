# 🔍 Vector Search Algorithms in C/C++

> Approximate Nearest Neighbor (ANN) search on high-dimensional vectors — implemented from scratch.

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue)
![Analysis](https://img.shields.io/badge/Analysis-Python-yellow)
![Dataset](https://img.shields.io/badge/Datasets-MNIST%20%7C%20SIFT-orange)
![Course](https://img.shields.io/badge/Course-Algorithm%20Development-purple)
![Team](https://img.shields.io/badge/Team-2%20members-green)

---

## About This Project

This project implements and benchmarks four **Approximate Nearest Neighbor (ANN)** algorithms
for searching high-dimensional vectors. The algorithms are evaluated on two real-world datasets
(MNIST and SIFT) and compared across key performance metrics: **QPS (Queries Per Second)**,
**Recall**, and **Approximation Factor (AF)**.

This is the 1st Programming Assignment for the course *"Software Development for Algorithmic Problems"*.

---

## Team

| Name | Student ID |
|------|-----------|
| Παπαθανασίου Ελένη | _1115202200135_ |
| Τόντου Αλτάνη-Δάφνη | 1115202200288 |

---

## Algorithms Implemented

| Algorithm | Description |
|-----------|-------------|
| **LSH** | Locality Sensitive Hashing — hash-based ANN search |
| **Hypercube** | Random projection onto a hypercube for fast ANN |
| **IVFFlat** | Inverted File index with flat (exact) distance computation |
| **IVFPQ** | Inverted File index with Product Quantization for compressed search |

---

## Tech Stack

![C++](https://img.shields.io/badge/C%2B%2B-g%2B%2B-blue)
![Python](https://img.shields.io/badge/Python-3.x-yellow)
![pandas](https://img.shields.io/badge/pandas-data%20analysis-lightgrey)
![matplotlib](https://img.shields.io/badge/matplotlib-plotting-lightgrey)
![Make](https://img.shields.io/badge/Build-Makefile-lightgrey)

- **C++** (g++) — algorithm implementation
- **Python 3** — experimental analysis & plotting (`pandas`, `matplotlib`, `seaborn`, `numpy`)
- **Makefile** — build system
- **Bash** — automated experiment scripts

---

## Project Structure

```
📁 src/
   ├── lsh.cpp           → LSH algorithm
   ├── hypercube.cpp     → Hypercube algorithm
   ├── IVFFlat.cpp       → IVFFlat algorithm
   ├── ivfpq.cpp         → IVFPQ algorithm
   ├── ivfpq_index.cpp   → IVFPQ auxiliary index structure
   ├── k_means.cpp       → k-means clustering (used by IVF methods)
   ├── mnist_data.cpp    → MNIST dataset loader
   ├── sift_data.cpp     → SIFT dataset loader
   └── main.cpp

📁 include/
   └── *.hpp             → Header files for all modules

📁 experiment/
   ├── parse_results.py  → Extract metrics from output files
   └── create_plot.py    → Generate comparison plots

📄 Makefile
📄 run_sift.sh           → SIFT experiment runner
📄 run_mnist.sh          → MNIST experiment runner
```

---

## Build & Run

### Compile

```bash
make all
```

### Run individual algorithms

```bash
make run_lsh
make run_hypercube
make run_ivfflat
make run_ivfpq
```

### Run experimental analysis (SIFT)

```bash
make sift_bash
make run_sift_bash
make run_parse_results
make run_create_plots
```

### Run experimental analysis (MNIST)

```bash
make mnist_bash
make run_mnist_bash
make run_parse_results
make run_create_plots
```

### Clean build files

```bash
make clean
```

---

## Experimental Output

Running the analysis pipeline generates the following files:

**CSV reports:**
- `algorithm_comparison_table.csv`
- `final_metrics_comparison.csv`
- `detailed_summary.csv`

**Plots:**
- `algorithm_comparison.png`
- `qps_by_algorithm.png` — queries per second per algorithm
- `recall_by_algorithm.png` — recall per algorithm
- `qps_vs_recall.png` — speed vs accuracy tradeoff
- `qps_vs_af.png` — speed vs approximation factor
- `af_vs_recall_correlation.png`
- `lsh_parameter_sensitivity.png`
- `correlation_matrix.png`

---

## System Requirements

- **OS:** Linux
- **C++ compiler:** g++
- **Python 3** with: `pandas`, `matplotlib`, `seaborn`, `numpy`

---

## Key Concepts Demonstrated

- Approximate Nearest Neighbor search in high-dimensional spaces
- Locality Sensitive Hashing (LSH) theory and implementation
- Product Quantization for vector compression
- k-means clustering from scratch
- Performance benchmarking: QPS, Recall, Approximation Factor
- Experimental analysis and data visualization

---

*1st Programming Assignment · Software Development for Algorithmic Problems*
