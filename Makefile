# Makefile for Boolean Function Minimization (Project 2)

CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall
DEBUGFLAGS = -std=c++11 -g -Wall -O0  # Debug flags: -g for debug info, -O0 for no optimization

TARGET = minimize
DEBUG_TARGET = minimize_debug
SRCS = $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)
DEBUG_OBJS = $(SRCS:.cpp=_debug.o)

# Default target
all: $(TARGET)

# Build executable (Release)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Build executable (Debug with symbols)
debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CXX) $(DEBUGFLAGS) -o $(DEBUG_TARGET) $(DEBUG_OBJS)

# Compile source files (Release)
src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile source files (Debug)
src/%_debug.o: src/%.cpp
	$(CXX) $(DEBUGFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(TARGET) $(DEBUG_TARGET) $(OBJS) $(DEBUG_OBJS)
	rm -f output_pla/*.pla
	rm -rf dot/ png/ build/

# Test with default input.pla (alias: make run)
test: $(TARGET)
	@mkdir -p output_pla
	./$(TARGET) pla_files/input.pla output_pla/input_output.pla
	@echo ""
	@echo "=========================================="
	@echo "  Output saved to: output_pla/input_output.pla"
	@echo "=========================================="
	@cat output_pla/input_output.pla

# Alias for test
run: test

# Test with debug version
test-debug: debug
	@mkdir -p output_pla
	./$(DEBUG_TARGET) pla_files/input.pla output_pla/input_output.pla

# Pattern rule to run any .pla file from pla_files directory
# Usage: make input.pla
# Usage: make test1.pla
%.pla: $(TARGET)
	@mkdir -p output_pla
	@if [ -f pla_files/$@ ]; then \
		BASENAME=$$(basename $@ .pla); \
		OUTPUT_FILE=output_pla/$${BASENAME}_output.pla; \
		echo "========================================"; \
		echo "  Running: pla_files/$@"; \
		echo "  Output: $${OUTPUT_FILE}"; \
		echo "========================================"; \
		./$(TARGET) pla_files/$@ $${OUTPUT_FILE}; \
		echo ""; \
		echo "========================================"; \
		echo "  Output File Content:"; \
		echo "========================================"; \
		cat $${OUTPUT_FILE}; \
	else \
		echo "Error: pla_files/$@ not found!"; \
		echo "Available files in pla_files/:"; \
		ls -1 pla_files/*.pla 2>/dev/null || echo "  (no .pla files found)"; \
		exit 1; \
	fi

.PHONY: all debug clean test test-debug run