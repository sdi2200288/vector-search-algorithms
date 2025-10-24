# Compiler και flags
CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -Iinclude

# Executable
TARGET = search

# Source files (όλα τα .cpp μέσα στο src)
SRCS = $(wildcard main4.cpp ivfpq.cpp k_means.cpp ivfpq_index.cpp ../src/sift_data.cpp ../src/mnist_data.cpp) 
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
run:
	./$(TARGET) -d input.dat -q query.dat -kclusters 100 -nprobe 10 -M 16 -o output.dat -N 1 -R 700 -type sift -nbits 8 -range true -ivfpq -seed 1

# Clean
clean:
	rm -f $(OBJS) $(TARGET) output.dat

.PHONY: all run clean