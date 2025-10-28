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
	./$(TARGET) -d "../sift/sift_base.fvecs" -q "../sift/sift_query.fvecs" -k 10 -L 30 -w 1.5 -o lsh_output.dat -N 5 -R 200 -type sift -range true -lsh

run_hypercube:
	./$(TARGET) -d "../sift/sift_base.fvecs" -q "../sift/sift_query.fvecs" -kproj 8 -w 1.0 -M 100 -probes 20 -o hyp_output.dat -N 5 -R 200 -type sift -range false -hypercube

run_ivfflat:
	./$(TARGET) -d "../sift/sift_base.fvecs" -q "../sift/sift_query.fvecs" -kclusters 256 -nprobe 20 -o ivfflat_output.dat -N 10 -R 30 -type sift -range false -ivfflat -seed 1

run_ivfpq:
	./$(TARGET) -d "../sift/sift_base.fvecs" -q "../sift/sift_query.fvecs" -kclusters 100 -nprobe 10 -M 16 -o output.dat -N 1 -R 700 -type sift -nbits 8 -range true -ivfpq -seed 1

# Clean
clean:
	rm -f $(OBJS) $(TARGET) output.dat lsh_output.dat hyp_output.dat ivfflat_output.dat

.PHONY: all run clean
