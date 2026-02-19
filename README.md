# mercurialcoredetector
A robust C library to detect silent hardware corruption in distributed training clusters. It uses triple modular redundancy and consensus checks to create verifiable checkpoints of program state, safeguarding against corrupted gradients during synchronization.

# Mercurial Core Detector (MCD)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

MCD protects distributed training clusters from silent data corruption caused by faulty GPUs (“mercurial cores”).  
It creates robust checkpoints of critical data (e.g., gradients) using three independent checksum algorithms and a majority vote.  
If corruption occurs during computation or synchronization, the verification step will detect it.

## Why traditional defences fail

- ECC memory cannot catch corruption that happens during computation.
- Checksums like CRC rely on vector instructions, which are themselves vulnerable.
- Any detection tool running on the same flawed hardware is suspect.

MCD uses only simple integer operations and triple redundancy to minimise the risk of common‑mode failures.

## Usage

Include `checkpoint.h` in your code:

```c
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
