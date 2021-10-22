#ifndef PREFETCH
#define PREFETCH(mem)  __builtin_prefetch(mem)
#endif

// uint8_t, uint32_t, int32_t
#include <stdint.h>
// size_t
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// When starting a series of partial CRC accumulations, use this starting CRC value
#define CRC32_FAST_SEED 0xFFFFFFFF

// TODO: this is the only place we use the crc32_fast nomenclature instead of fast_cksum

/// compute CRC32
uint32_t crc32_fast(const void* data, size_t length);

/// Compute a CRC32 on a chunk(s) of data and then generate the final CRC32 by combining with the total length
uint32_t crc32_fast_partial(const void* data, size_t length, uint32_t previousCrc32 = CRC32_FAST_SEED);
uint32_t crc32_fast_finalize(size_t total_length, uint32_t previousCrc32);

// Use a specific CRC32 chunking algorithm
uint32_t crc32_fast_16bytes(const void* data, size_t length);
// Use a specific partial algorithm
uint32_t crc32_fast_16bytes_partial(const void* data, size_t length, uint32_t previousCrc32 = CRC32_FAST_SEED);

#ifdef __cplusplus
}  // extern "C"
#endif
