# Compiler και flags
CXX = g++
CXXFLAGS = -Wall -g -std=c++17

# Executable
TARGET = search

# Source files
SRCS = main.cpp lsh.cpp
OBJS = $(SRCS:.cpp=.o)

# Default target: build μόνο
all: $(TARGET)

# Compile .cpp files σε .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link objects σε executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Run με τα επιχειρήματα σου
run:
	./$(TARGET) -d input.dat -q query.dat -k 4 -L 5 -w 4.0 -o output.txt -N 1 -R 2000 -type mnist -range true -lsh

# Clean
clean:
	rm -f $(OBJS) $(TARGET) sift

.PHONY: all run clean
