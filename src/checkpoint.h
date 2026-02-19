#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <stdint.h>
#include <stddef.h>

// Save a checksum of the given data as a checkpoint.
// Returns 0 on success, -1 if consensus could not be reached (possible hardware error).
int save_checkpoint(const uint8_t* data, size_t len);

// Verify current data against the last saved checkpoint.
// Returns:
//   0  – verification passed.
//  -1  – consensus error during verification (checksum computation may be corrupt).
//  -2  – no checkpoint has been saved.
//  -3  – checkpoint mismatch (corruption detected).
int verify_checkpoint(const uint8_t* data, size_t len);

#endif // CHECKPOINT_H
