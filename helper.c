#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"

// Function to reverse byte order (necessary for Little-Endian to Big-Endian conversion)
void reverse_bytes(uint8_t *data, size_t size) {
    size_t i;
    for (i = 0; i < size / 2; i++) {
        uint8_t temp = data[i];
        data[i] = data[size - 1 - i];
        data[size - 1 - i] = temp;
    }
}

// Converts 4-byte SyncSafe integer to a standard 32-bit integer
uint32_t decode_syncsafe(uint32_t syn_int) {
    uint32_t decoded = 0;
    
    // Bits 7-14-21-28 are used, the MSB of each byte is ignored
    decoded |= (syn_int & 0x7F000000) >> 3;
    decoded |= (syn_int & 0x007F0000) >> 2;
    decoded |= (syn_int & 0x00007F00) >> 1;
    decoded |= (syn_int & 0x0000007F);

    return decoded;
}

// Converts a standard 32-bit integer to 4-byte SyncSafe
uint32_t encode_syncsafe(uint32_t std_int) {
    uint32_t encoded = 0;
    
    // Shift data into the lower 7 bits of each byte
    encoded |= (std_int & 0x0000007F);
    encoded |= (std_int & 0x00003F80) << 1;
    encoded |= (std_int & 0x001FC000) << 2;
    encoded |= (std_int & 0x00E00000) << 3;

    return encoded;
}

// Checks for the "ID3" identifier at the start of the file
bool is_valid_id3(FILE *fp) {
    char identifier[4];
    fseek(fp, 0, SEEK_SET);
    if (fread(identifier, 1, 3, fp) != 3) {
        return false;
    }
    identifier[3] = '\0';
    return (strcmp(identifier, "ID3") == 0);
}