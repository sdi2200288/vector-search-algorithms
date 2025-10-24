# Compiler και flags
CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -Iinclude

# Executable
TARGET = search

# Source files (όλα τα .cpp μέσα στο src)
SRCS = $(wildcard main4.cpp ivfpq.cpp ../src/sift_data.cpp) 
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
	./$(TARGET) -d input.dat -q query.dat -kclusters 50 -nprobe 5 -M 16 -o output.dat -N 1 -R 200 -type sift -nbits 8 -range true -ivfpq -seed 1

# Clean
clean:
	rm -f $(OBJS) $(TARGET) output.dat

.PHONY: all run clean