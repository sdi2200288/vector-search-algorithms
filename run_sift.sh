#!/bin/bash

DATASET="../sift/sift_base.fvecs"
QUERY="../sift/sift_query.fvecs"
type="sift"
OUTPUT_DIR=sift_output

mkdir -p $OUTPUT_DIR


#LSH

# Διάφορα k values
for k in 4 6 8; do
    echo "Running LSH with k=$k"
    ./search -d $DATASET -q $QUERY -k $k -L 30 -w 1.5 -o $OUTPUT_DIR/lsh_k${k}.dat -N 5 -R 200 -type $type -range true -lsh
done

# Διάφορα L values  
for L in 3 5 10; do
    echo "Running LSH with L=$L"
    ./search -d $DATASET -q $QUERY -k 4 -L $L -w 1.5 -o $OUTPUT_DIR/lsh_L${L}.dat -N 5 -R 200 -type $type -range true -lsh
done

# Διάφορα w values
for w in 1.5 2.0 2.5; do
    echo "Running LSH with w=$w"
    ./search -d $DATASET -q $QUERY -k 4 -L 30 -w $w -o $OUTPUT_DIR/lsh_w${w}.dat -N 5 -R 200 -type $type -range true -lsh
done

#HYPERCUBE

# Διάφορα k values
for kproj in 10 14 16; do
    echo "Running Hypercube with kproj=$kproj"
    ./search -d $DATASET -q $QUERY -kproj $kproj -w 1.0 -M 100 -probes 20 -o $OUTPUT_DIR/hypercube_kproj${kproj}.dat -N 5 -R 200 -type sift -range false -hypercube
done

# Διάφορα L values  
for probes in 2 5 10; do
    echo "Running Hypercube with kproj=$kproj"
    ./search -d $DATASET -q $QUERY -kproj 8 -w 1.0 -M 100 -probes $probes -o $OUTPUT_DIR/hypercube_probes${$probes}.dat -N 5 -R 200 -type sift -range false -hypercube
done

# Διάφορα w values
for w in 4.0 4.5; do
    echo "Running Hypercube with w=$w"
    ./search -d $DATASET -q $QUERY -kproj 8 -w $w -M 100 -probes 20 -o $OUTPUT_DIR/hypercube_w${w}.dat -N 5 -R 200 -type sift -range false -hypercube
done

# Διάφορα Μ values
for M in 100 150; do
    echo "Running Hypercube with M=$M"
    ./search -d $DATASET -q $QUERY -kproj 8 -w 1.0 -M $M -probes 20 -o $OUTPUT_DIR/hypercube_M${M}.dat -N 5 -R 200 -type sift -range false -hypercube
done


#IVFFLAT

for nprobe in 5 10 20 40; do
    ./search -d $DATASET -q $QUERY -kclusters 256 -nprobe $nprobe -o "$OUTPUT_DIR/ivfflat_nprobe${nprobe}.dat" -N 10 -R 30 -type sift -ivfflat
done

# IVFPQ 

for nprobe in 5 10 20; do
    ./search -d $DATASET -q $QUERY -kclusters 256 -nprobe $nprobe -M 8 -nbits 8 -o "$OUTPUT_DIR/ivfpq_nprobe${nprobe}.dat" -N 10 -R 30 -type sift -ivfpq
done

for M in 4 8 16; do
    ./search -d $DATASET -q $QUERY -kclusters 256 -nprobe 10 -M $M -nbits 8 -o "$OUTPUT_DIR/ivfpq_M${M}.dat" -N 10 -R 30 -type sift -ivfpq
done