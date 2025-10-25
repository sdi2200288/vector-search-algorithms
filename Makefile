# Compiler και flags
CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -Iinclude

# Executable
TARGET = search

# Source files (όλα τα .cpp μέσα στο src)
SRCS = $(wildcard src/main.cpp src/lsh.cpp src/hypercube.cpp src/IVFFlat.cpp src/ivfpq.cpp src/ivfpq_index.cpp src/k_means.cpp src/sift_data.cpp src/mnist_data.cpp) 
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Compile .cpp -> .o
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Run
run_lsh:
	./$(TARGET) -d "../mnist_data/t10k-images-idx3-ubyte" -q "../mnist_data/train-images-idx3-ubyte" -k 10 -L 30 -w 1.5 -o output.dat -N 5 -R 150.0 -type sift -range true -lsh

run_hypercube:
	./$(TARGET) -d input.dat -q query.dat -kproj 8 -w 1.0 -M 100 -probes 20 -o output.dat -N 5 -R 200 -type sift -range true -hypercube

run_ivfflat:
	./$(TARGET) -d input.dat -q query.dat -kclusters 100 -nprobe 10 -o output.dat -N 10 -R 2000 -type mnist -range true -ivfflat -seed 1

run_ivfpq:
	./$(TARGET) -d input.dat -q query.dat -kclusters 100 -nprobe 10 -M 16 -o output.dat -N 1 -R 700 -type sift -nbits 8 -range true -ivfpq -seed 1

# Clean
clean:
	rm -f $(OBJS) $(TARGET) output.dat

.PHONY: all run clean
