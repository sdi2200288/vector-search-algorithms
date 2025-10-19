# Compiler και flags
CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -Iinclude

# Executable
TARGET = search

# Source files (όλα τα .cpp μέσα στο src)
SRCS = $(wildcard main2.cpp hypercube.cpp ../src/sift_data.cpp) 
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
	./$(TARGET) -d input.dat -q query.dat -kproj 8 -w 1.0 -M 100 -probes 20 -o output.dat -N 5 -R 200 -type sift -range true -hypercube

# Clean
clean:
	rm -f $(OBJS) $(TARGET) output.dat

.PHONY: all run clean