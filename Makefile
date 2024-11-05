# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -O3 -march=native -mtune=native -ffast-math \
            -flto -funroll-loops -fno-math-errno \
            -fomit-frame-pointer -fno-trapping-math -fexpensive-optimizations \
			-mavx512f -mavx512bw -mavx512dq -mavx512vl \
            -std=c++17 -Wall -Wextra -Wpedantic -Wuninitialized -Wmaybe-uninitialized

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
$(TARGET): $(BUILD_DIR) $(OUTPUT_DIR) $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(TARGET) $(SRC) -static-libgcc -static-libstdc++

# Run the program after building
run: $(TARGET)
	@./$(TARGET)

# Clean up build and output directories
clean:
	rm -rf build/ output/

# Clean, build, and run in sequence
rebuild: clean $(TARGET)

# Clean, build, and run in one step
rebuild-run: rebuild run

# Phony targets (non-file targets)
.PHONY: clean run rebuild rebuild-run
