#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read.h"
#include "helper.h"

// Reads the ID3v2.3 Header
bool read_id3_header(FILE *fp, ID3Header *header) {
    uint8_t buffer[ID3_HEADER_SIZE];
    
    fseek(fp, 0, SEEK_SET);
    if (fread(buffer, 1, ID3_HEADER_SIZE, fp) != ID3_HEADER_SIZE) {
        return false;
    }

    // Extract raw size (4 bytes)
    uint32_t raw_size;
    memcpy(&raw_size, buffer + 6, 4);
    reverse_bytes((uint8_t *)&raw_size, 4); // Handle Big-Endian to Little-Endian conversion

    // Populate header struct
    memcpy(header->identifier, buffer, 3);
    header->identifier[3] = '\0';
    header->version_major = buffer[3];
    header->version_revision = buffer[4];
    header->flags = buffer[5];
    header->size = decode_syncsafe(raw_size);

    return true;
}

// Parses the next frame and extracts data if it's a target frame (TIT2 or TPE1)
bool parse_next_frame(FILE *fp, uint32_t tag_end_pos, TagData *tag_data) {
    ID3FrameHeader frame_header;
    long current_pos = ftell(fp);

    if (current_pos >= tag_end_pos) {
        return false; // Reached end of tag
    }

    uint8_t buffer[ID3_FRAME_HEADER_SIZE];
    if (fread(buffer, 1, ID3_FRAME_HEADER_SIZE, fp) != ID3_FRAME_HEADER_SIZE) {
        return false; 
    }

    // Check for padding/empty frames
    if (buffer[0] == 0) {
        fseek(fp, tag_end_pos, SEEK_SET); // Skip remaining padding
        return false;
    }

    // Extract Frame ID and Size
    memcpy(frame_header.frame_id, buffer, 4);
    frame_header.frame_id[4] = '\0';

    uint32_t raw_size;
    memcpy(&raw_size, buffer + 4, 4);
    reverse_bytes((uint8_t *)&raw_size, 4);
    frame_header.size = raw_size;

    // --- Process TIT2 Frame ---
    if (strcmp(frame_header.frame_id, "TIT2") == 0) {
        tag_data->title_pos = current_pos;
        tag_data->title_size_raw = ID3_FRAME_HEADER_SIZE + frame_header.size;
        
        uint8_t encoding;
        if (fread(&encoding, 1, 1, fp) == 1) { // Read encoding byte
            uint32_t text_size = frame_header.size - 1; 
            if (text_size < MAX_TITLE_LEN) {
                fread(tag_data->title, 1, text_size, fp);
                tag_data->title[text_size] = '\0';
            }
        }
    } 
    // --- Process TPE1 (Artist) Frame <--- NEW LOGIC
    else if (strcmp(frame_header.frame_id, "TPE1") == 0) {
        tag_data->artist_pos = current_pos;
        tag_data->artist_size_raw = ID3_FRAME_HEADER_SIZE + frame_header.size;
        
        uint8_t encoding;
        if (fread(&encoding, 1, 1, fp) == 1) { // Read encoding byte
            uint32_t text_size = frame_header.size - 1; 
            if (text_size < MAX_TITLE_LEN) {
                fread(tag_data->artist, 1, text_size, fp);
                tag_data->artist[text_size] = '\0';
            }
        }
    }

    // Skip the rest of the frame content to the next frame header
    fseek(fp, current_pos + ID3_FRAME_HEADER_SIZE + frame_header.size, SEEK_SET);

    return true;
}

// Main function to read tags
bool read_tags_from_file(const char *filepath, TagData *tag_data) {
    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        return false;
    }
    
    // Initialize tag data (important for string termination)
    memset(tag_data, 0, sizeof(TagData));

    if (!is_valid_id3(fp)) {
        fclose(fp);
        return false;
    }

    ID3Header header;
    if (!read_id3_header(fp, &header)) {
        fclose(fp);
        return false;
    }
    
    uint32_t tag_end_pos = ID3_HEADER_SIZE + header.size;
    
    // Loop through all frames
    while (parse_next_frame(fp, tag_end_pos, tag_data)) {
        // Continue parsing frames
    }

    fclose(fp);
    return true;
}