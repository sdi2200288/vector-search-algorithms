# # Compiler και flags
# CXX = g++
# CXXFLAGS = -Wall -g -std=c++17

# # Executable
# TARGET = search

# # Source files
# SRCS = main.cpp lsh.cpp
# OBJS = $(SRCS:.cpp=.o)

# # Default target: build μόνο
# all: $(TARGET)

# # Compile .cpp files σε .o
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# # Link objects σε executable
# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# # Run με τα επιχειρήματα σου
# run:
# 	./$(TARGET) -d input.dat -q query.dat -k 4 -L 5 -w 4.0 -o output.txt -N 1 -R 2000 -type mnist -range true -lsh

# # Clean
# clean:
# 	rm -f $(OBJS) $(TARGET) sift mnist

# .PHONY: all run clean


# Compiler και flags
CXX = g++
CXXFLAGS = -Wall -g -std=c++17 -Iinclude

# Executable
TARGET = search

# Source files (όλα τα .cpp μέσα στο src)
SRCS = $(wildcard src/main.cpp src/lsh.cpp src/sift_data.cpp) #src/mnist_data.cpp 
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
	./$(TARGET) -d input.dat -q query.dat -k 10 -L 30 -w 1.5 -o output.txt -N 5 -R 150.0 -type sift -range true -lsh

# Clean
clean:
	rm -f $(OBJS) $(TARGET) src/mnist_data.o src/sift_data.o src/main1

.PHONY: all run clean
