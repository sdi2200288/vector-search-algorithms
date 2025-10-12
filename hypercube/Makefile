# Compiler και flags
CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -Iinclude

# Executable
TARGET = search

# Source files (όλα τα .cpp μέσα στο src)
SRCS = $(wildcard main2.cpp ) 
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
	./$(TARGET) -d input.dat -q query.dat -kproj 14 -w 4.0 -M 10 -probes 2 -o output.dat -N 1 -R 2000 -type mnist -range true -hypercube

# Clean
clean:
	rm -f $(OBJS) $(TARGET) 

.PHONY: all run clean