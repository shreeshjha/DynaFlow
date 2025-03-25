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
│   └── Hybrid-Approach (Slow-Fast-Path).png
└── src
    ├── dataset_gen.c
    ├── hybrid_feedback.c
    ├── hybrid_immediate.c
    └── traditional.c
```
