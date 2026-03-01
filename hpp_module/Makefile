# H++ Configuration - Makefile for Recursive Ontology Engine
# Compiler: g++ with C++17 standard
# Author: 1proprogrammerchant

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native -flto
DEBUG_FLAGS = -g -O0 -DDEBUG
TARGET = recursive_ontology
SOURCE = recursive_ontology.cpp

# Build targets
.PHONY: all clean debug run

all: $(TARGET)

$(TARGET): $(SOURCE)
	@echo "Building H++ Recursive Ontology Engine (Optimized)..."
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: clean $(TARGET)
	@echo "Debug build complete with symbols"

run: $(TARGET)
	@echo "Running H++ Ontology Engine:"
	./$(TARGET)

clean:
	@echo "Cleaning build artifacts..."
	rm -f $(TARGET) *.o *.out
	@echo "Clean complete"

# Performance profiling build
profile: CXXFLAGS += -pg
profile: clean $(TARGET)
	@echo "Profile build complete"
