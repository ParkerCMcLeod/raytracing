# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -static

# Target executable
TARGET = build/raytracer

# Source files
SRC = src/raytracer.cpp

# Include directories
INCLUDE = -Iinclude

# Output directory
OUTPUT_DIR = output

# Ensure build and output directories exist
BUILD_DIR = build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# Build the target executable with static linking
$(TARGET): $(BUILD_DIR) $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(TARGET) $(SRC) -static-libgcc -static-libstdc++

# Run the program after building
run: $(TARGET)
	@./$(TARGET)

# Clean up build files
clean:
	rm -rf build/ output/

# Phony targets (non-file targets)
.PHONY: clean run
