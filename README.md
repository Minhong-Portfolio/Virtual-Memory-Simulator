# Virtual Memory Simulator

## Description

This project implements a byte-addressed virtual memory simulator for a simplified operating system. It handles virtual-to-physical address translation, page faults, and page replacement algorithms, and collects statistics on memory operations.

Key functionalities include:

* Splitting virtual addresses into VPN and offset
* Initializing system and per-process page tables
* Translating memory accesses with read/write support
* Generating page faults and handling them via swap or zero-fill
* Three page replacement policies: Random, Approximate LRU, and FIFO
* Periodic LRU "daemon" to age referenced pages
* Computing statistics: total accesses, page faults, writebacks, and AMAT

## Features

* **Address Splitting**: Extract VPN and offset using bit masks
* **MMU Setup**: `system_init()`, `proc_init()`, `context_switch()`
* **Memory Access**: `mem_access()` with read/write and fault detection
* **Page Fault Handling**: `page_fault()` allocates or swaps pages
* **Page Replacement**:

  * Random
  * Approximate LRU (bit-shift aging)
  * FIFO
* **Statistics**: Tracks accesses, faults, writebacks, and calculates AMAT

## Prerequisites

* GCC (C99)
* Make
* Linux or macOS environment

## Building

```bash
# Clone the repository
# Build the simulator and tests
make
```

## Usage

```bash
# Basic simulation on a trace file with Random replacement
./vm-sim -i traces/simple.trace -rrandom

# Use Approximate LRU or FIFO
./vm-sim -i traces/simple.trace -rlru
./vm-sim -i traces/simple.trace -rfifo

# Enable strict memory corruption checking
./vm-sim -i traces/simple.trace -rrandom -c

# Run unit tests for core components
./vm-sim -t
```

### Command-Line Options

* `-i <file>` : Specify trace file (required)
* `-r{random|lru|fifo}` : Select replacement algorithm (required)
* `-s` : Read trace from stdin
* `-c` : Enable strict memory corruption checking
* `-t` : Run built-in unit tests
* `-h` : Print help message

## Example Output

```
Total Accesses     : 1000000
Page Faults        : 2000
Writes to disk     : 500
Average Memory Access Time: 350.123000
Max Swap Size      : 64 KB
```

## Project Structure

```
├── address_splitting.h    # Inline helpers for VPN/offset and page table lookups
├── mmu.c                  # Core memory management unit functions
├── page_fault.c           # Page fault handling logic
├── page_replacement.c     # free_frame() and replacement policies
├── proc.c                 # Process initialization and context switching
├── stats.c                # Statistics calculation (AMAT)
├── swapops.c              # Swap space enqueue/dequeue/read/write
├── tests/                 # Unit tests for address splitting, stats, replacement
├── traces/                # Example trace files for simulation
├── Makefile               # Build rules
└── README.md              # Project overview and instructions
```

*Implementer: Minhong Cho (GTID: 903806863)*
