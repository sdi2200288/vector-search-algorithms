# Compiler και flags
CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -Iinclude

# Executable
TARGET = search

# Source files (όλα τα .cpp μέσα στο src)
SRCS = $(wildcard main3.cpp IVFFlat.cpp ../src/sift_data.cpp ../src/mnist_data.cpp) 
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
	./$(TARGET) -d input.dat -q query.dat -kclusters 100 -nprobe 10 -o output.dat -N 10 -R 2000 -type mnist -range true -ivfflat -seed 1

# Clean
clean:
	rm -f $(OBJS) $(TARGET) 

.PHONY: all run clean