# Makefile for Hybrid Flow Processor
# Alternative to CMake for simple compilation

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -O3 -march=native -Wall -Wextra
LDFLAGS = -lm
DEBUG_FLAGS = -O0 -g -DDEBUG

# Source files
FLOW_PROCESSOR_SRC = src/hybrid_accelerated.c
DATASET_GENERATOR_SRC = multi_dataset_tester.c

# Executables
FLOW_PROCESSOR = hybrid_accelerated
DATASET_GENERATOR = multi_dataset_generator

# Test files
TEST_SCRIPT = automated_tester.sh
COMPILE_SCRIPT = compile_and_test.sh

# Default target
all: $(FLOW_PROCESSOR) $(DATASET_GENERATOR)
	@echo "✅ Build completed successfully!"
	@echo "🚀 Ready to test your flow processor!"
	@echo ""
	@echo "Available targets:"
	@echo "  make generate_datasets - Generate test datasets"
	@echo "  make test_quick       - Run quick test"
	@echo "  make test_all         - Run comprehensive tests"
	@echo "  make clean            - Clean build files"

# Flow processor compilation
$(FLOW_PROCESSOR): $(FLOW_PROCESSOR_SRC)
	@echo "🔨 Compiling flow processor..."
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✅ Flow processor compiled successfully"

# Dataset generator compilation
$(DATASET_GENERATOR): $(DATASET_GENERATOR_SRC)
	@echo "🔨 Compiling dataset generator..."
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
	@echo "✅ Dataset generator compiled successfully"

# Debug builds
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(FLOW_PROCESSOR) $(DATASET_GENERATOR)
	@echo "🐛 Debug build completed"

# Generate all test datasets
generate_datasets: $(DATASET_GENERATOR)
	@echo "📊 Generating test datasets..."
	./$(DATASET_GENERATOR)
	@echo "✅ Datasets generated successfully"

# Quick test on uniform dataset
test_quick: $(FLOW_PROCESSOR) generate_datasets
	@echo "⚡ Running quick test..."
	cp dataset_uniform.txt dataset.txt
	./$(FLOW_PROCESSOR)
	@echo "✅ Quick test completed"

# Comprehensive testing
test_all: $(FLOW_PROCESSOR) $(DATASET_GENERATOR)
	@if [ -f "$(TEST_SCRIPT)" ]; then \
		echo "🔬 Running comprehensive testing..."; \
		chmod +x $(TEST_SCRIPT); \
		./$(TEST_SCRIPT); \
	else \
		echo "❌ $(TEST_SCRIPT) not found"; \
		echo "Please create $(TEST_SCRIPT) for comprehensive testing"; \
	fi

# Test individual datasets
test_web: $(FLOW_PROCESSOR) generate_datasets
	@echo "🌐 Testing web traffic dataset..."
	cp dataset_web.txt dataset.txt
	./$(FLOW_PROCESSOR)

test_ddos: $(FLOW_PROCESSOR) generate_datasets
	@echo "⚔️ Testing DDoS simulation dataset..."
	cp dataset_ddos.txt dataset.txt
	./$(FLOW_PROCESSOR)

test_streaming: $(FLOW_PROCESSOR) generate_datasets
	@echo "📺 Testing video streaming dataset..."
	cp dataset_streaming.txt dataset.txt
	./$(FLOW_PROCESSOR)

test_iot: $(FLOW_PROCESSOR) generate_datasets
	@echo "🌐 Testing IoT sensor dataset..."
	cp dataset_iot.txt dataset.txt
	./$(FLOW_PROCESSOR)

# Setup and initialization
setup:
	@echo "📁 Setting up project structure..."
	mkdir -p src
	mkdir -p build
	mkdir -p test_results
	@if [ -f "$(COMPILE_SCRIPT)" ]; then \
		chmod +x $(COMPILE_SCRIPT); \
	fi
	@if [ -f "$(TEST_SCRIPT)" ]; then \
		chmod +x $(TEST_SCRIPT); \
	fi
	@echo "✅ Project setup completed"

# Performance benchmarking
benchmark: $(FLOW_PROCESSOR) generate_datasets
	@echo "📊 Running performance benchmark..."
	@echo "Testing uniform dataset (baseline)..."
	@cp dataset_uniform.txt dataset.txt
	@time ./$(FLOW_PROCESSOR) > benchmark_uniform.txt 2>&1
	@echo "Testing web traffic dataset..."
	@cp dataset_web.txt dataset.txt  
	@time ./$(FLOW_PROCESSOR) > benchmark_web.txt 2>&1
	@echo "Testing datacenter dataset..."
	@cp dataset_datacenter.txt dataset.txt
	@time ./$(FLOW_PROCESSOR) > benchmark_datacenter.txt 2>&1
	@echo "✅ Benchmark completed - check benchmark_*.txt files"

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f $(FLOW_PROCESSOR) $(DATASET_GENERATOR)
	rm -f dataset_*.txt dataset.txt
	rm -f benchmark_*.txt
	rm -rf test_results
	rm -rf build
	@echo "✅ Clean completed"

# Install (copy to system directories)
install: $(FLOW_PROCESSOR) $(DATASET_GENERATOR)
	@echo "📦 Installing executables..."
	sudo cp $(FLOW_PROCESSOR) /usr/local/bin/
	sudo cp $(DATASET_GENERATOR) /usr/local/bin/
	@echo "✅ Installation completed"

# Uninstall
uninstall:
	@echo "🗑️ Uninstalling executables..."
	sudo rm -f /usr/local/bin/$(FLOW_PROCESSOR)
	sudo rm -f /usr/local/bin/$(DATASET_GENERATOR)
	@echo "✅ Uninstallation completed"

# Help target
help:
	@echo "🔧 Available Make targets:"
	@echo ""
	@echo "Build targets:"
	@echo "  all              - Build all executables (default)"
	@echo "  debug            - Build with debug symbols"
	@echo "  clean            - Remove build artifacts"
	@echo ""
	@echo "Dataset targets:"
	@echo "  generate_datasets - Generate all test datasets"
	@echo ""
	@echo "Testing targets:"
	@echo "  test_quick       - Quick test on uniform dataset"
	@echo "  test_all         - Comprehensive multi-dataset testing"
	@echo "  test_web         - Test web traffic patterns"
	@echo "  test_ddos        - Test DDoS simulation"
	@echo "  test_streaming   - Test video streaming patterns"
	@echo "  test_iot         - Test IoT sensor patterns"
	@echo "  benchmark        - Run performance benchmark"
	@echo ""
	@echo "Setup targets:"
	@echo "  setup            - Setup project directories"
	@echo "  install          - Install to system directories"
	@echo "  uninstall        - Remove from system directories"
	@echo "  help             - Show this help message"

# Phony targets
.PHONY: all debug clean generate_datasets test_quick test_all test_web test_ddos test_streaming test_iot setup benchmark install uninstall help

# Default shell
SHELL := /bin/bash
