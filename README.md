# DynaFlow: Adaptive Fast/Slow-Path Network Simulator

![Hybrid Approach (Slow-Fast-Path)](docs/Hybrid-Approach%20(Slow-Fast-Path).png)

DynaFlow is an experimental project that demonstrates an adaptive fast-path/slow-path architecture for network flow processing. The project compares a traditional approach with a hybrid approach that immediately learns unknown flows to reduce the overhead of expensive operations.

## Overview

In high-throughput network systems, most packets are processed via a fast path with minimal overhead. However, a small fraction require deeper, more expensive inspection (the slow path). DynaFlow simulates these scenarios using two different implementations:

- **Traditional Approach:** Processes each packet by checking a static set of known flows. Packets not in this set always trigger a slow path operation.
- **Hybrid Approach:** Uses an immediate learning strategy. When a packet is processed via the slow path, its flow is added to the known flows so that subsequent packets can be processed via the fast path.

The project includes a dataset generator to ensure that both approaches run on the same dataset, enabling a fair performance comparison.

## Directory Structure

```text
├── CMakeLists.txt
├── docs
│   └── Hybrid-Approach (Slow-Fast-Path).png  # Diagram illustrating the hybrid approach
└── src
    ├── dataset_gen.c  # Program to generate the test dataset
    ├── hybrid_feedback.c
    ├── hybrid_immediate.c  # Hybrid approach with immediate learning
    └── traditional.c  # Traditional fast/slow-path implementation
```


## Getting Started

### Prerequisites

- **CMake** (version 3.10 or higher)
- A C compiler (e.g., gcc)

### Building the Project

1. **Clone the Repository:**
   ```bash
   git clone <repository-url> DynaFlow
   cd DynaFlow
   ```
2. Create a Build Directory and Build:
   ```bash
   mkdir build
    cd build
    cmake ..
    cmake --build .
   ```
   This will build three executables:
   - dataset_gen
   - traditional
   - hybrid_immediate
  
### Generating the Dataset
To generate a common dataset for both approaches, run:
```bash
./dataset_gen
```
This will create a file called `dataset.txt in the` project root. Both the traditional and hybrid programs read from this file.

### Running the Approaches

1. Traditional Approach:
```bash
./traditional
```
2. Hybrid Approach (Immediate Learning):
```bash
./hybrid_immediate
```

Both programs will output metrics such as the number of slow-path triggers and the total processing time.

## Project Details
- **Dataset Generation:**
The `dataset_gen.c` program generates a dataset containing:
    - A fixed number of known flows.
    - A sequence of packets (IP addresses) representing network traffic.
    - The dataset is written to dataset.txt for a fair comparison.

- **Traditional Approach:**
    Reads from the dataset and processes each packet. Unknown flows always trigger the slow path.

- **Hybrid Approach:**
    Also reads from the dataset. When a packet triggers the slow path, its flow is immediately added to the known flows to avoid repeated slow path processing for future packets.

- **Performance Metrics:**
Both approaches print out:
    - The number of slow path triggers.
    - Total time taken.
    - Other configuration details (e.g., dataset size).

## Benchmark Results

Performance testing was conducted by running each implementation 10 times on `dataset.txt`.

### Traditional Slow-Fast Path

```
            Mean        Std.Dev.    Min         Median      Max
real        12.831      0.075       12.763      12.807      13.004
user        12.782      0.033       12.744      12.774      12.850
sys         0.026       0.006       0.018       0.025       0.037
```

### Hybrid Slow-Fast Path

```
            Mean        Std.Dev.    Min         Median      Max
real        12.556      0.073       12.430      12.547      12.718
user        12.503      0.041       12.400      12.514      12.544
sys         0.031       0.012       0.015       0.030       0.057
```

## Performance Comparison

The hybrid slow-fast path implementation shows a **2.14%** improvement in real execution time compared to the traditional approach:
- Traditional implementation: 12.831s mean execution time
- Hybrid implementation: 12.556s mean execution time

## Usage

To run the benchmarks yourself:
For benchmarking (not ideal benchmark) I am using [Multitime](https://tratt.net/laurie/src/multitime/releases.html)

```bash
cd build
# Run traditional implementation for n = 10 (example)
multitime -n 10 ./traditional

# Run hybrid implementation
multitime -n 10 ./hybrid_immediate
```


### References
[The Slow Path Needs an Accelerator](https://dl.acm.org/doi/10.1145/3594255.3594259)
