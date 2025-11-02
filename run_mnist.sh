#!/bin/bash

DATASET="../mnist_data/train-images-idx3-ubyte"
QUERY="../mnist_data/t10k-images-idx3-ubyte"
type="mnist"
OUTPUT_DIR=mnist_output

mkdir -p $OUTPUT_DIR

# LSH

Διάφορα k values
for k in 4 6; do
    echo "Running LSH with k=$k"
    ./search -d $DATASET -q $QUERY -k $k -L 30 -w 1.5 -o $OUTPUT_DIR/lsh_k${k}.dat -N 5 -R 5 -type $type -range true -lsh
done

# Διάφορα L values  
for L in 3 5; do
    echo "Running LSH with L=$L"
    ./search -d $DATASET -q $QUERY -k 4 -L $L -w 1.5 -o $OUTPUT_DIR/lsh_L${L}.dat -N 5 -R 5 -type $type -range true -lsh
done

# Διάφορα w values
for w in 1.5 2.0; do
    echo "Running LSH with w=$w"
    ./search -d $DATASET -q $QUERY -k 4 -L 30 -w $w -o $OUTPUT_DIR/lsh_w${w}.dat -N 5 -R 5 -type $type -range true -lsh
done

HYPERCUBE

# Διάφορα k values
for kproj in 10 14; do
    echo "Running Hypercube with kproj=$kproj"
    ./search -d $DATASET -q $QUERY -kproj $kproj -w 1.0 -M 100 -probes 20 -o $OUTPUT_DIR/hypercube_kproj${kproj}.dat -N 5 -R 2 -type $type -range true -hypercube
done

# Διάφορα L values  
for probes in 2 5; do
    echo "Running Hypercube with kproj=$kproj"
    ./search -d $DATASET -q $QUERY -kproj 8 -w 1.0 -M 100 -probes $probes -o $OUTPUT_DIR/hypercube_probes${$probes}.dat -N 5 -R 2 -type $type -range true -hypercube
done

# Διάφορα w values
for w in 4.0 4.5; do
    echo "Running Hypercube with w=$w"
    ./search -d $DATASET -q $QUERY -kproj 8 -w $w -M 100 -probes 20 -o $OUTPUT_DIR/hypercube_w${w}.dat -N 5 -R 2 -type $type -range true -hypercube
done

# Διάφορα Μ values
for M in 100 150; do
    echo "Running Hypercube with M=$M"
    ./search -d $DATASET -q $QUERY -kproj 8 -w 1.0 -M $M -probes 20 -o $OUTPUT_DIR/hypercube_M${M}.dat -N 5 -R 2 -type $type -range true -hypercube
done

./search -d $DATASET -q $QUERY -kproj 8 -w 1.0 -M 150 -probes 20 -o $OUTPUT_DIR/hypercube_M150.dat -N 5 -R 2 -type $type -range true -hypercube

#IVFFLAT

for nprobe in 5 10; do
    ./search -d $DATASET -q $QUERY -kclusters 256 -nprobe $nprobe -o "$OUTPUT_DIR/ivfflat_nprobe${nprobe}.dat" -N 10 -R 30 -type $type -ivfflat
done

# IVFPQ 
./search -d $DATASET -q $QUERY -kclusters 100 -nprobe 5 -M 8 -o "$OUTPUT_DIR/ivfpq_nprobe{5}.dat" -N 10 -R 30 -type $type -nbits 8 -range true -ivfpq -seed 1

# IVFPQ με διάφορα kclusters (προαιρετικά)
for kclusters in 64 128; do
    echo "Running IVFPQ with kclusters=$kclusters"
    ./search -d $DATASET -q $QUERY -kclusters $kclusters -nprobe 5 -M 8 -o "$OUTPUT_DIR/ivfpq_k${kclusters}.dat" -N 10 -R 30 -type $type -nbits 8 -range true -ivfpq -seed 1
done