# Makefile for Boolean Function Minimization (Project 2)

CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall

TARGET = minimize
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile source files
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(OBJS)
	rm -f *.pla output.pla
	rm -rf dot/ png/ build/

# Test with example files
test: $(TARGET)
	./$(TARGET) pla_files/test1.pla output.pla

.PHONY: all clean test