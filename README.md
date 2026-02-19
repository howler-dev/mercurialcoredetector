# Mercurial Core Detector (MCD)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A clean C library to detect silent hardware corruption in distributed training clusters. It uses triple modular redundancy and consensus checks to create verifiable checkpoints of program state, safeguarding against corrupted gradients during synchronization.

MCD protects distributed training clusters from silent data corruption caused by faulty GPUs (“mercurial cores”).  
It creates checkpoints of critical data (e.g., gradients) using three independent checksum algorithms and a majority vote.  
If corruption occurs during computation or synchronization, the verification step will detect it.

## Why Traditional Defences Fail

- ECC memory cannot catch corruption that happens during computation.
- Checksums like CRC rely on vector instructions, which are themselves vulnerable.
- Any detection tool running on the same flawed hardware is suspect.

MCD uses only simple integer operations and triple redundancy to minimise the risk of common‑mode failures.

## Features
1. Triple Checksum Redundancy: Three algorithms (simple sum, linear hash, CRC-32) vote on the correct value.
2. Majority‑vote Consensus: If at least two checksums agree, the result is stored; otherwise an error is raised.
3. Checkpoint & Verify: Save a checkpoint before a critical operation, then verify afterwards.

NOTE: This is created with strict C coding, i.e., no recursion, bounded loops, no dynamic memory after init, function length ≤60 lines, assertions, checked return values, zero compiler warnings, and restricted pointer use.

## Repository Structure

```
mercurial-core-detector/
├── LICENSE
├── README.md
├── .gitignore
├── Makefile
├── src/
│   ├── checkpoint.h
│   └── checkpoint.c
└── tests/
    └── test_checkpoint.c
```

## Usage

Include `checkpoint.h` in your code:

```
#include "checkpoint.h"

// Example: save checkpoint of gradient data
uint8_t gradients[1024];
// ... fill gradients ...
if (save_checkpoint(gradients, sizeof(gradients)) != 0) {
    // handle error (possible hardware corruption)
}

// ... perform gradient synchronisation ...

// Verify that gradients haven't been corrupted
int ret = verify_checkpoint(gradients, sizeof(gradients));
if (ret == -3) {
    fprintf(stderr, "Corruption detected! Discard gradients.\n");
} else if (ret != 0) {
    fprintf(stderr, "Consensus error – checksum computation itself may be faulty.\n");
}
```
## Building
Run `make` to build the library and test executable.
The test program runs a series of checks and prints a summary.

```
$ make
gcc -Wall -Wextra -pedantic -std=c99 -I./src -c -o src/checkpoint.o src/checkpoint.c
gcc -Wall -Wextra -pedantic -std=c99 -I./src -c -o tests/test_checkpoint.o tests/test_checkpoint.c
gcc -o test_checkpoint src/checkpoint.o tests/test_checkpoint.o
$ ./test_checkpoint

All tests passed.
```
## Design Constraints (Enfored in Code)

- No recursion.
- Every loop has a provable upper bound.
- No dynamic memory allocation after initialisation.
- Maximum 60 lines per function.
- Minimum 2 assertions per function.
- Every return value checked.
- Zero compiler warnings (`-Wall -Wextra -pedantic`).
- No function pointers.
- Restricted pointer dereferencing (only simple reads).

For more clarity on these constraints, please see: `CONTRIBUTING.md`

## License

MIT

## Object Files
- *.o
- *.a
- *.so

## Executables
- test_checkpoint
- test

## Editor Files
- *.swp
- *.swo
- *~

## OS Files
- .DS_Store
- Thumbs.db

# How to Use MCD in a Distributed Training Cluster
1. On each worker after computing local gradients, call `save_checkpoint(gradients, size)`. Store the returned robust checksum (the internal `g_checkpoint` value) and send it along with the gradients to the parameter server.
2. On the parameter server upon receiving gradients, recompute their checksum using the same consensus logic (you could call the internal `consensus_checksum` if you expose it, or simply re‑run `save_checkpoint` on a temporary copy). Compare with the received checksum. If they differ, discard the gradients and mark the worker as suspicious.
3. After synchronization, workers can optionally verify that the averaged gradients received from the server match a precomputed expectation (if the server also sends a checksum). This creates a two‑way check against corruption during transmission or averaging.
