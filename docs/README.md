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
