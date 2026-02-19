#include "checkpoint.h"
#include <assert.h>

// Static checkpoint storage
static uint32_t g_checkpoint = 0;
static int g_checkpoint_valid = 0;

// Three diverse checksum algorithms (all use simple integer ops)
static uint32_t checksum_alg1(const uint8_t* data, size_t len) {
    assert(data != NULL);
    assert(len < 1024 * 1024);  // provable upper bound
    uint32_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += data[i];
    }
    return sum;
}

static uint32_t checksum_alg2(const uint8_t* data, size_t len) {
    assert(data != NULL);
    assert(len < 1024 * 1024);
    uint32_t hash = 0;
    for (size_t i = 0; i < len; i++) {
        hash = (hash << 5) - hash + data[i];  // simple linear hash
    }
    return hash;
}

static uint32_t checksum_alg3(const uint8_t* data, size_t len) {
    assert(data != NULL);
    assert(len < 1024 * 1024);
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {  // fixed 8 iterations per byte
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }
    }
    return ~crc;
}

// Compute consensus of three checksums. Returns 0 on success and stores majority in *result.
// Returns -1 if no majority (possible corruption in checksum computation itself).
static int consensus_checksum(const uint8_t* data, size_t len, uint32_t* result) {
    assert(data != NULL);
    assert(result != NULL);
    assert(len < 1024 * 1024);

    uint32_t c1 = checksum_alg1(data, len);
    uint32_t c2 = checksum_alg2(data, len);
    uint32_t c3 = checksum_alg3(data, len);

    if (c1 == c2 || c1 == c3) {
        *result = c1;
        return 0;
    }
    if (c2 == c3) {
        *result = c2;
        return 0;
    }
    return -1;  // all three differ → possible error
}

int save_checkpoint(const uint8_t* data, size_t len) {
    assert(data != NULL);
    assert(len < 1024 * 1024);

    uint32_t chk;
    int ret = consensus_checksum(data, len, &chk);
    if (ret != 0) return ret;  // propagate error

    g_checkpoint = chk;
    g_checkpoint_valid = 1;
    return 0;
}

int verify_checkpoint(const uint8_t* data, size_t len) {
    assert(data != NULL);
    assert(len < 1024 * 1024);

    if (!g_checkpoint_valid) return -2;  // no checkpoint saved

    uint32_t chk;
    int ret = consensus_checksum(data, len, &chk);
    if (ret != 0) return ret;

    if (chk != g_checkpoint) return -3;  // mismatch → corruption detected
    return 0;  // verification passed
}
