#ifndef HELPER_H
#define HELPER_H

#include "types.h"

// --- SyncSafe Converters ---
uint32_t decode_syncsafe(uint32_t syn_int);
uint32_t encode_syncsafe(uint32_t std_int);

// --- File/Memory Utilities ---
void reverse_bytes(uint8_t *data, size_t size);
bool is_valid_id3(FILE *fp);

#endif