#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include <stdbool.h>

// --- Constants ---
#define ID3_HEADER_SIZE 10
#define ID3_FRAME_HEADER_SIZE 10
#define MAX_TITLE_LEN 255
#define TEMP_SUFFIX "_temp"

// --- ID3v2.3 Header Structure (10 bytes) ---
typedef struct {
    char identifier[4];       // "ID3"
    uint8_t version_major;    // 03
    uint8_t version_revision; // 00
    uint8_t flags;
    uint32_t size;            // Total size of the tag, decoded SyncSafe
} ID3Header;

// --- ID3v2.3 Frame Header Structure (10 bytes) ---
typedef struct {
    char frame_id[5];         // e.g., "TIT2"
    uint32_t size;            // Frame size (standard integer)
    uint16_t flags;
} ID3FrameHeader;

// --- Main Data Structure for Tag Content ---
typedef struct {
    char title[MAX_TITLE_LEN + 1]; // TIT2
    uint32_t title_pos;            // Position of TIT2 frame (relative to start of file)
    uint32_t title_size_raw;       // Size of the original TIT2 frame

    char artist[MAX_TITLE_LEN + 1]; // TPE1 <--- NEW FIELD
    uint32_t artist_pos;           // Position of TPE1 frame
    uint32_t artist_size_raw;      // Size of the original TPE1 frame
} TagData;

#endif // TYPE_H