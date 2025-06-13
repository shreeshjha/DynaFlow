#!/bin/bash

# Enhanced Multi-Dataset Testing Script for DynaFlow
# Tests your hybrid flow processor on diverse network traffic patterns
# Expects datasets in 'tests' folder

echo "🚀 === ENHANCED DYNAFLOW MULTI-DATASET TESTING === 🚀"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Dataset files (simple array instead of associative array for compatibility)
datasets=(
    "tests/dataset_uniform.txt"
    "tests/dataset_web.txt"
    "tests/dataset_datacenter.txt"
    "tests/dataset_ddos.txt"
    "tests/dataset_streaming.txt"
    "tests/dataset_iot.txt"
    "tests/dataset_gaming.txt"
    "tests/dataset_cdn.txt"
    "tests/dataset_enterprise.txt"
    "tests/dataset_pareto.txt"
)

# Corresponding dataset names
dataset_names=(
    "Uniform Random (Baseline)"
    "Web Traffic (Zipf 80/20 Rule)"
    "Datacenter East-West Traffic"
    "DDoS Attack Simulation"
    "Video Streaming (Netflix-like)"
    "IoT Sensor Network"
    "Gaming Traffic (eSports/MMO)"
    "CDN Edge Traffic"
    "Enterprise Mixed Workload"
    "Pareto Heavy-Tail Distribution"
)

# Expected performance characteristics
expected_fast_path=(
    "40-50"
    "55-65"
    "60-70"
    "15-25"
    "65-75"
    "45-55"
    "50-60"
    "60-70"
    "45-55"
    "35-45"
)

# Results storage
RESULTS_DIR="test_results_$(date +%Y%m%d_%H%M%S)"
mkdir -p $RESULTS_DIR
SUMMARY_FILE="$RESULTS_DIR/performance_summary.txt"
DETAILED_LOG="$RESULTS_DIR/detailed_test.log"

# Start logging
exec > >(tee -a $DETAILED_LOG)
exec 2>&1

echo "📁 Setting up test environment..."
echo "   Project directory: $(pwd)"
echo "   Results directory: $RESULTS_DIR"
echo "   Detailed log: $DETAILED_LOG"
echo ""

# Create tests directory if it doesn't exist
if [ ! -d "tests" ]; then
    echo "📁 Creating tests directory..."
    mkdir -p tests
fi

# Check and compile dataset generator
echo "🔧 Checking dataset generator..."
if [ ! -f "multi_dataset_generator" ]; then
    echo "   Compiling multi_dataset_tester.c..."
    gcc -o multi_dataset_generator multi_dataset_tester.c -lm -O2
    if [ $? -ne 0 ]; then
        echo -e "${RED}❌ Failed to compile dataset generator${NC}"
        exit 1
    fi
    echo -e "${GREEN}✅ Dataset generator compiled successfully${NC}"
else
    echo -e "${GREEN}✅ Dataset generator already exists${NC}"
fi

# Check if datasets exist in tests folder
datasets_exist=0
for dataset_file in "${datasets[@]}"; do
    if [ -f "$dataset_file" ]; then
        datasets_exist=$((datasets_exist + 1))
    fi
done

# Generate datasets if they don't exist or if less than half exist
if [ $datasets_exist -lt 5 ]; then
    echo ""
    echo "📊 Generating diverse network traffic datasets in tests folder..."
    
    # Run the original generator and then move files to tests folder
    ./multi_dataset_generator
    
    # Move generated files to tests folder if they were created in root
    for file in dataset_*.txt; do
        if [ -f "$file" ]; then
            mv "$file" "tests/"
            echo "   Moved $file to tests/"
        fi
    done
    
    echo -e "${GREEN}✅ Datasets generated in tests folder${NC}"
else
    echo -e "${GREEN}✅ Using existing datasets in tests folder${NC}"
fi

# Check and compile flow processor
echo ""
echo "🧪 Checking flow processor..."
if [ ! -f "src/hybrid_accelerated" ]; then
    echo "   Compiling hybrid_accelerated.c..."
    cd src
    gcc -o hybrid_accelerated hybrid_accelerated.c -lm -O3 -march=native
    if [ $? -ne 0 ]; then
        echo -e "${RED}❌ Failed to compile flow processor${NC}"
        exit 1
    fi
    cd ..
    echo -e "${GREEN}✅ Flow processor compiled successfully${NC}"
else
    echo -e "${GREEN}✅ Flow processor already exists${NC}"
fi

# Verify all required files exist
echo ""
echo "🔍 Verifying test setup..."
missing_files=0
for dataset_file in "${datasets[@]}"; do
    if [ ! -f "$dataset_file" ]; then
        echo -e "${RED}❌ Missing dataset: $dataset_file${NC}"
        missing_files=$((missing_files + 1))
    fi
done

if [ $missing_files -gt 0 ]; then
    echo -e "${RED}❌ $missing_files dataset files are missing${NC}"
    echo "Available datasets:"
    ls -la tests/dataset_*.txt 2>/dev/null || echo "No datasets found in tests folder"
    exit 1
fi

if [ ! -f "src/hybrid_accelerated" ]; then
    echo -e "${RED}❌ Flow processor executable not found${NC}"
    exit 1
fi

echo -e "${GREEN}✅ All required files found${NC}"

echo ""
echo "🔄 Starting comprehensive testing on $(date)..."
echo "   Testing ${#datasets[@]} different traffic patterns"
echo "   Results will be saved to $RESULTS_DIR/"
echo ""

# Initialize summary file with enhanced format
cat > $SUMMARY_FILE << EOF
=== DYNAFLOW ENHANCED MULTI-DATASET PERFORMANCE ANALYSIS ===
Generated on: $(date)
Processor: $(uname -m) $(uname -s)
Test Environment: $(pwd)
Dataset Location: tests/

PERFORMANCE SUMMARY TABLE:
================================================================================================================================
Dataset                   | Throughput | Fast%  | Accel% | Ultra% | Slow%  | Cache% | ML Acc% | Time(s) | Flows  | Status
                          | (Mpps)     |        |        |        |        |        |         |         |        |       
================================================================================================================================
EOF

# Test each dataset with enhanced monitoring
test_count=0
total_tests=${#datasets[@]}
failed_tests=0
best_throughput=0
best_dataset=""

for i in "${!datasets[@]}"; do
    dataset_file="${datasets[$i]}"
    dataset_name="${dataset_names[$i]}"
    expected_range="${expected_fast_path[$i]}"
    
    test_count=$((test_count + 1))
    
    echo -e "${BLUE}🔬 Test $test_count/$total_tests: $dataset_name${NC}"
    echo "   📁 Dataset: $dataset_file"
    echo "   📈 Expected fast path: $expected_range%"
    
    if [ ! -f "$dataset_file" ]; then
        echo -e "${RED}❌ Dataset file not found${NC}"
        failed_tests=$((failed_tests + 1))
        continue
    fi
    
    # Run the flow processor with the dataset file as argument
    output_file="$RESULTS_DIR/output_$(basename $dataset_file .txt).txt"
    
    echo "   ⚡ Running flow processor with $dataset_file..."
    start_time=$(date +%s)
    
    # Check if processor supports command line args, otherwise copy file
    timeout 300s ./src/hybrid_accelerated "$dataset_file" > "$output_file" 2>&1
    exit_code=$?
    
    # If that failed, try the old way
    if [ $exit_code -ne 0 ]; then
        echo "   🔄 Trying fallback method (copying dataset)..."
        cp "$dataset_file" src/dataset.txt
        cd src
        timeout 300s ./hybrid_accelerated > "../$output_file" 2>&1
        exit_code=$?
        cd ..
    fi
    
    end_time=$(date +%s)
    test_duration=$((end_time - start_time))
    
    if [ $exit_code -eq 124 ]; then
        echo -e "${RED}⚠️  Test timed out after 5 minutes${NC}"
        echo "TIMEOUT - Test exceeded 300 seconds" >> "$output_file"
        failed_tests=$((failed_tests + 1))
        printf "%-25s | %-10s | %-6s | %-6s | %-6s | %-6s | %-6s | %-7s | %-7s | %-6s | TIMEOUT\n" \
            "$(echo $dataset_name | cut -c1-24)" "TIMEOUT" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" ">300" "N/A" >> $SUMMARY_FILE
        continue
    elif [ $exit_code -ne 0 ]; then
        echo -e "${RED}❌ Test failed with exit code $exit_code${NC}"
        failed_tests=$((failed_tests + 1))
        printf "%-25s | %-10s | %-6s | %-6s | %-6s | %-6s | %-6s | %-7s | %-7s | %-6s | FAILED\n" \
            "$(echo $dataset_name | cut -c1-24)" "FAILED" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "$test_duration" "N/A" >> $SUMMARY_FILE
        continue
    fi
    
    # Extract comprehensive metrics from output
    if [ -f "$output_file" ]; then
        # Core performance metrics
        throughput=$(grep "Throughput:" "$output_file" | grep -o '[0-9]*\.[0-9]*' | head -1)
        total_time=$(grep "Total Processing Time:" "$output_file" | grep -o '[0-9]*\.[0-9]*')
        
        # Path distribution - Updated regex patterns for your output format
        fast_path=$(grep -E "(Fast|Fast Path):" "$output_file" | grep -o '([0-9]*\.[0-9]*%)' | tr -d '()%' | head -1)
        accel_path=$(grep -E "(Accelerated|Accelerated Path):" "$output_file" | grep -o '([0-9]*\.[0-9]*%)' | tr -d '()%' | head -1)
        ultra_path=$(grep -E "(Ultra-Fast|Ultra Fast):" "$output_file" | grep -o '([0-9]*\.[0-9]*%)' | tr -d '()%' | head -1)
        slow_path=$(grep -E "(Slow|Slow Path):" "$output_file" | grep -o '([0-9]*\.[0-9]*%)' | tr -d '()%' | head -1)
        
        # Cache and ML metrics
        cache_hit=$(grep "Cache Hit Rate:" "$output_file" | grep -o '[0-9]*\.[0-9]*' | head -1)
        ml_accuracy=$(grep "Validation Accuracy:" "$output_file" | grep -o '[0-9]*\.[0-9]*' | head -1)
        total_flows=$(grep "Total Flows Created:" "$output_file" | grep -o '[0-9]*' | head -1)
        
        # Performance analysis
        status="PASS"
        if [ -n "$fast_path" ] && [ -n "$expected_range" ]; then
            min_expected=$(echo $expected_range | cut -d'-' -f1)
            max_expected=$(echo $expected_range | cut -d'-' -f2)
            if command -v bc &> /dev/null; then
                if (( $(echo "$fast_path < $min_expected" | bc -l) )) || (( $(echo "$fast_path > $max_expected" | bc -l) )); then
                    status="WARN"
                fi
            fi
        fi
        
        # Track best performance
        if [ -n "$throughput" ] && command -v bc &> /dev/null; then
            if (( $(echo "$throughput > $best_throughput" | bc -l) )); then
                best_throughput=$throughput
                best_dataset=$dataset_name
            fi
        fi
        
        # Display results with color coding
        echo -e "${GREEN}✅ Test completed in ${test_duration}s${NC}"
        echo "   📊 Throughput: ${throughput:-N/A} Mpps"
        echo "   🚀 Fast path: ${fast_path:-N/A}% (expected: $expected_range%)"
        echo "   💾 Cache hits: ${cache_hit:-N/A}%"
        echo "   🧠 ML accuracy: ${ml_accuracy:-N/A}%"
        echo "   📦 Total flows: ${total_flows:-N/A}"
        
        if [ "$status" = "WARN" ]; then
            echo -e "${YELLOW}⚠️  Fast path percentage outside expected range${NC}"
        fi
        
        # Add to summary with proper formatting
        printf "%-25s | %-10s | %-6s | %-6s | %-6s | %-6s | %-6s | %-7s | %-7s | %-6s | %s\n" \
            "$(echo $dataset_name | cut -c1-24)" \
            "${throughput:-N/A}" \
            "${fast_path:-N/A}" \
            "${accel_path:-N/A}" \
            "${ultra_path:-N/A}" \
            "${slow_path:-N/A}" \
            "${cache_hit:-N/A}" \
            "${ml_accuracy:-N/A}" \
            "${total_time:-N/A}" \
            "${total_flows:-N/A}" \
            "$status" >> $SUMMARY_FILE
    else
        echo -e "${RED}❌ No output file generated${NC}"
        failed_tests=$((failed_tests + 1))
        printf "%-25s | %-10s | %-6s | %-6s | %-6s | %-6s | %-6s | %-7s | %-7s | %-6s | NO_OUTPUT\n" \
            "$(echo $dataset_name | cut -c1-24)" "ERROR" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "$test_duration" "N/A" >> $SUMMARY_FILE
    fi
    
    echo ""
    sleep 1  # Brief pause between tests
done

# Add summary footer
cat >> $SUMMARY_FILE << EOF
================================================================================================================================

TEST EXECUTION SUMMARY:
- Total tests: $total_tests
- Successful: $((total_tests - failed_tests))
- Failed/Timeout: $failed_tests
- Best performance: $best_dataset ($best_throughput Mpps)
- Test duration: $(date)

DATASET LOCATIONS:
$(ls -la tests/dataset_*.txt 2>/dev/null | head -10)

EOF

echo "📊 Generating advanced performance analysis..."

# Create enhanced Python analysis script
cat > "$RESULTS_DIR/enhanced_analysis.py" << 'EOF'
#!/usr/bin/env python3
import os
import re
import sys
import json
from collections import defaultdict

def extract_comprehensive_metrics(filename):
    """Extract comprehensive metrics from output file"""
    metrics = {}
    
    try:
        with open(filename, 'r') as f:
            content = f.read()
        
        # Basic performance metrics
        throughput_match = re.search(r'Throughput: ([\d.]+) Mpps', content)
        metrics['throughput'] = float(throughput_match.group(1)) if throughput_match else 0
        
        time_match = re.search(r'Total Processing Time: ([\d.]+) seconds?', content)
        if not time_match:
            time_match = re.search(r'Total time: ([\d.]+) s', content)
        metrics['time'] = float(time_match.group(1)) if time_match else 0
        
        # Path distribution - More flexible regex
        fast_match = re.search(r'Fast[^:]*:\s*\d+\s*\(([\d.]+)%\)', content)
        metrics['fast_path'] = float(fast_match.group(1)) if fast_match else 0
        
        accel_match = re.search(r'Accelerated[^:]*:\s*\d+\s*\(([\d.]+)%\)', content)
        metrics['accel_path'] = float(accel_match.group(1)) if accel_match else 0
        
        ultra_match = re.search(r'Ultra[^:]*:\s*\d+\s*\(([\d.]+)%\)', content)
        metrics['ultra_path'] = float(ultra_match.group(1)) if ultra_match else 0
        
        slow_match = re.search(r'Slow[^:]*:\s*\d+\s*\(([\d.]+)%\)', content)
        metrics['slow_path'] = float(slow_match.group(1)) if slow_match else 0
        
        # Cache performance
        cache_match = re.search(r'Cache Hit Rate: ([\d.]+)%', content)
        metrics['cache_hit'] = float(cache_match.group(1)) if cache_match else 0
        
        # ML metrics
        ml_accuracy_match = re.search(r'Validation Accuracy: ([\d.]+)%', content)
        metrics['ml_accuracy'] = float(ml_accuracy_match.group(1)) if ml_accuracy_match else 0
        
        # Flow metrics
        flows_match = re.search(r'Total Flows Created: (\d+)', content)
        metrics['total_flows'] = int(flows_match.group(1)) if flows_match else 0
        
    except Exception as e:
        print(f"Error processing {filename}: {e}")
        
    return metrics

def create_enhanced_analysis():
    """Create comprehensive analysis with advanced metrics"""
    
    # Dataset information
    datasets = {
        'output_dataset_uniform.txt': {'name': 'Uniform Random', 'type': 'baseline'},
        'output_dataset_web.txt': {'name': 'Web Traffic', 'type': 'realistic'},
        'output_dataset_datacenter.txt': {'name': 'Datacenter', 'type': 'enterprise'},
        'output_dataset_ddos.txt': {'name': 'DDoS Attack', 'type': 'security'},
        'output_dataset_streaming.txt': {'name': 'Video Streaming', 'type': 'media'},
        'output_dataset_iot.txt': {'name': 'IoT Sensors', 'type': 'iot'},
        'output_dataset_gaming.txt': {'name': 'Gaming', 'type': 'interactive'},
        'output_dataset_cdn.txt': {'name': 'CDN Edge', 'type': 'cdn'},
        'output_dataset_enterprise.txt': {'name': 'Enterprise', 'type': 'business'},
        'output_dataset_pareto.txt': {'name': 'Pareto Heavy-tail', 'type': 'mathematical'}
    }
    
    # Extract metrics for all datasets
    all_metrics = {}
    dataset_types = defaultdict(list)
    
    for filename, info in datasets.items():
        if os.path.exists(filename):
            metrics = extract_comprehensive_metrics(filename)
            if metrics and metrics.get('throughput', 0) > 0:
                all_metrics[info['name']] = metrics
                dataset_types[info['type']].append(info['name'])
    
    if not all_metrics:
        print("❌ No valid metrics found")
        print("Available files:", [f for f in os.listdir('.') if f.startswith('output_')])
        return
    
    print("\n" + "="*100)
    print("🎯 DYNAFLOW ENHANCED PERFORMANCE ANALYSIS")
    print("="*100)
    
    # Overall performance summary
    total_datasets = len(all_metrics)
    avg_throughput = sum(m['throughput'] for m in all_metrics.values()) / total_datasets
    max_throughput = max(m['throughput'] for m in all_metrics.values())
    min_throughput = min(m['throughput'] for m in all_metrics.values())
    
    print(f"\n📈 OVERALL PERFORMANCE SUMMARY:")
    print(f"   Datasets tested: {total_datasets}")
    print(f"   Average throughput: {avg_throughput:.2f} Mpps")
    print(f"   Peak throughput: {max_throughput:.2f} Mpps")
    print(f"   Throughput range: {min_throughput:.2f} - {max_throughput:.2f} Mpps")
    print(f"   Performance variance: {((max_throughput - min_throughput) / avg_throughput * 100):.1f}%")
    
    # Throughput comparison with enhanced visualization
    print(f"\n🚀 THROUGHPUT PERFORMANCE (Mpps):")
    print("-" * 80)
    
    for label, metrics in sorted(all_metrics.items(), key=lambda x: x[1]['throughput'], reverse=True):
        throughput = metrics['throughput']
        bar_length = int((throughput / max_throughput) * 40) if max_throughput > 0 else 0
        bar = "█" * bar_length + "░" * (40 - bar_length)
        efficiency = (throughput / max_throughput) * 100 if max_throughput > 0 else 0
        print(f"{label:18} │{bar}│ {throughput:6.2f} ({efficiency:4.1f}%)")
    
    # Performance recommendations
    print(f"\n💡 PERFORMANCE INSIGHTS & RECOMMENDATIONS:")
    print("-" * 80)
    
    best_overall = max(all_metrics.items(), key=lambda x: x[1]['throughput'])
    best_cache = max(all_metrics.items(), key=lambda x: x[1].get('cache_hit', 0))
    most_efficient_fast = max(all_metrics.items(), key=lambda x: x[1].get('fast_path', 0))
    
    print(f"🏆 Best overall performance: {best_overall[0]} ({best_overall[1]['throughput']:.2f} Mpps)")
    print(f"💾 Best cache performance: {best_cache[0]} ({best_cache[1].get('cache_hit', 0):.1f}%)")
    print(f"🚀 Most fast path usage: {most_efficient_fast[0]} ({most_efficient_fast[1].get('fast_path', 0):.1f}%)")
    
    # Save detailed metrics to JSON
    with open('detailed_metrics.json', 'w') as f:
        json.dump(all_metrics, f, indent=2)
    
    print(f"\n📁 Detailed metrics saved to: detailed_metrics.json")

if __name__ == "__main__":
    create_enhanced_analysis()
EOF

# Run enhanced analysis if Python is available
echo ""
if command -v python3 &> /dev/null; then
    echo -e "${PURPLE}🐍 Running enhanced performance analysis...${NC}"
    cd $RESULTS_DIR
    python3 enhanced_analysis.py
    cd ..
    echo ""
else
    echo -e "${YELLOW}ℹ️  Python3 not available - skipping detailed analysis${NC}"
fi

# Create quick shell-based analysis as fallback
echo -e "${CYAN}📈 Quick Performance Analysis:${NC}"
echo ""

# Count successful tests
successful_tests=0
total_throughput=0
for i in "${!datasets[@]}"; do
    dataset_file="${datasets[$i]}"
    output_file="$RESULTS_DIR/output_$(basename $dataset_file .txt).txt"
    if [ -f "$output_file" ]; then
        throughput=$(grep "Throughput:" "$output_file" | grep -o '[0-9]*\.[0-9]*' | head -1)
        if [ -n "$throughput" ] && [ "$throughput" != "0.00" ]; then
            successful_tests=$((successful_tests + 1))
            if command -v bc &> /dev/null; then
                total_throughput=$(echo "$total_throughput + $throughput" | bc -l 2>/dev/null || echo "$total_throughput")
            fi
        fi
    fi
done

if [ $successful_tests -gt 0 ]; then
    if command -v bc &> /dev/null; then
        avg_throughput=$(echo "scale=2; $total_throughput / $successful_tests" | bc -l)
        echo "   ✅ Successful tests: $successful_tests/$total_tests"
        echo "   📊 Average throughput: $avg_throughput Mpps"
    else
        echo "   ✅ Successful tests: $successful_tests/$total_tests"
        echo "   📊 Total throughput: $total_throughput Mpps"
    fi
else
    echo "   ❌ No successful tests completed"
fi

echo ""
echo -e "${GREEN}🎉 Multi-dataset testing completed!${NC}"
echo ""

# Display results summary
echo -e "${BLUE}📊 RESULTS SUMMARY:${NC}"
echo ""
cat $SUMMARY_FILE
echo ""

echo -e "${YELLOW}📁 OUTPUT FILES:${NC}"
echo "   • Test results directory: $RESULTS_DIR/"
echo "   • Performance summary: $SUMMARY_FILE"
echo "   • Individual test outputs: $RESULTS_DIR/output_dataset_*.txt"
if [ -f "$RESULTS_DIR/detailed_metrics.json" ]; then
    echo "   • Detailed metrics (JSON): $RESULTS_DIR/detailed_metrics.json"
fi
echo ""

echo -e "${CYAN}💡 NEXT STEPS:${NC}"
echo "   1. Review individual test outputs for optimization opportunities"
echo "   2. Compare performance across different traffic patterns"
echo "   3. Focus on improving slow path usage in DDoS scenarios"
echo "   4. Optimize cache performance for uniform random traffic"
echo "   5. Fine-tune ML model based on validation accuracy results"
echo ""

# Final recommendations based on results
if [ $successful_tests -gt 5 ]; then
    echo -e "${GREEN}🚀 EXCELLENT: Most tests completed successfully!${NC}"
    echo "   Your flow processor shows good performance across diverse workloads."
elif [ $successful_tests -gt 2 ]; then
    echo -e "${YELLOW}⚠️  GOOD: Some tests completed successfully.${NC}"
    echo "   Consider investigating failed tests for potential improvements."
else
    echo -e "${RED}❌ NEEDS ATTENTION: Few tests completed successfully.${NC}"
    echo "   Review compilation settings and dataset generation."
fi

echo ""
echo -e "${GREEN}✨ DynaFlow multi-dataset testing complete! ✨${NC}"
