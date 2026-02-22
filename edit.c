#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "edit.h"
#include "read.h"
#include "helper.h"

// Creates the full ID3v2.3 frame byte array for TIT2
size_t create_tit2_frame(uint8_t *buffer, const char *title) 
{
    // Content = Encoding (1 byte) + Text (Variable)
    size_t title_len = strlen(title);
    uint32_t content_size = title_len + 1; 
    
    // Frame ID (TIT2)
    memcpy(buffer, "TIT2", 4);
    
    // Size (4 bytes, Big-Endian)
    uint32_t raw_size = content_size;
    reverse_bytes((uint8_t *)&raw_size, 4); 
    memcpy(buffer + 4, &raw_size, 4);
    
    // Flags (2 bytes, 0x0000)
    memset(buffer + 8, 0, 2);
    
    // Content: Encoding (0x03 for UTF-8) + Text
    buffer[10] = 0x03; 
    memcpy(buffer + 11, title, title_len);
    
    return ID3_FRAME_HEADER_SIZE + content_size;
}

// Creates the full ID3v2.3 frame byte array for TPE1 (Artist) <--- NEW FUNCTION
size_t create_tpe1_frame(uint8_t *buffer, const char *artist) 
{
    size_t artist_len = strlen(artist);
    uint32_t content_size = artist_len + 1; 
    
    // Frame ID (TPE1)
    memcpy(buffer, "TPE1", 4);
    
    // Size (4 bytes, Big-Endian)
    uint32_t raw_size = content_size;
    reverse_bytes((uint8_t *)&raw_size, 4); 
    memcpy(buffer + 4, &raw_size, 4);
    
    // Flags (2 bytes, 0x0000)
    memset(buffer + 8, 0, 2);
    
    // Content: Encoding (0x03 for UTF-8) + Text
    buffer[10] = 0x03; 
    memcpy(buffer + 11, artist, artist_len);
    
    return ID3_FRAME_HEADER_SIZE + content_size;
}


// Function to perform the actual file rewrite (Refactored from original edit_tag_title)
static bool rewrite_file_with_new_frame(const char *filepath, const TagData *old_tag_data,
                                        const ID3Header *old_header,
                                        const char *frame_id, 
                                        const uint8_t *new_frame_buffer, size_t new_frame_size,
                                        uint32_t old_frame_pos, uint32_t old_frame_raw_size) 
{
    char temp_filepath[256];
    strcpy(temp_filepath, filepath);
    strcat(temp_filepath, TEMP_SUFFIX);
    
    // Calculate new tag size
    // 1. Tag content size without the old frame
    uint32_t tag_content_size_old = old_header->size;
    if (old_frame_raw_size > 0) {
        tag_content_size_old -= old_frame_raw_size;
    }
    
    // 2. New total tag size
    uint32_t new_tag_size_decoded = tag_content_size_old + new_frame_size;
    uint32_t new_tag_size_encoded = encode_syncsafe(new_tag_size_decoded);
    
    FILE *fp_in = fopen(filepath, "rb");
    FILE *fp_out = fopen(temp_filepath, "wb");
    if (!fp_in || !fp_out) {
        if (fp_in) fclose(fp_in);
        if (fp_out) fclose(fp_out);
        perror("Error opening files for rewrite");
        return false;
    }

    // --- 3a. Write the new ID3 Header ---
    uint8_t header_buffer[ID3_HEADER_SIZE];
    memcpy(header_buffer, "ID3", 3);
    header_buffer[3] = 0x03; // V2.3
    header_buffer[4] = 0x00; // Revision 0
    header_buffer[5] = 0x00; // Flags
    
    uint32_t raw_size_be = new_tag_size_encoded; // Already SyncSafe Big-Endian
    memcpy(header_buffer + 6, &raw_size_be, 4);
    
    fwrite(header_buffer, 1, ID3_HEADER_SIZE, fp_out);

    // --- 3b. Write the new frame (TIT2 or TPE1) ---
    fwrite(new_frame_buffer, 1, new_frame_size, fp_out);
    
    // --- 3c. Copy remaining frames and audio ---
    long data_start_pos = old_header->size + ID3_HEADER_SIZE;
    
    // The point in the old file right after the frame we replaced
    long old_frame_end_pos = old_frame_pos + old_frame_raw_size;
    
    // Start copying from the beginning of the audio data if we couldn't find the old frame
    long copy_start_pos = (old_frame_raw_size > 0) ? old_frame_end_pos : ID3_HEADER_SIZE;
    
    // If the old frame was found, we seek to copy all frames that followed it
    if (fseek(fp_in, copy_start_pos, SEEK_SET) != 0) {
        printf("Error: Failed to seek input file.\n");
        fclose(fp_in);
        fclose(fp_out);
        return false;
    }

    // Copy remaining tag data + audio data
    char copy_buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(copy_buffer, 1, 4096, fp_in)) > 0) {
        fwrite(copy_buffer, 1, bytes_read, fp_out);
    }

    // --- 4. CLEANUP and RENAME ---
    fclose(fp_in);
    fclose(fp_out);

    if (remove(filepath) != 0) {
        perror("Error deleting original file");
        return false;
    }
    if (rename(temp_filepath, filepath) != 0) {
        perror("Error renaming temporary file");
        return false;
    }

    return true;
}


// Main function to edit the title tag (uses the rewrite helper)
bool edit_tag_title(const char *filepath, const char *new_title) {
    TagData old_tag_data;
    if (!read_tags_from_file(filepath, &old_tag_data)) {
        printf("Error: Could not read original tag data.\n");
        return false;
    }
    
    ID3Header old_header;
    FILE *fp_in = fopen(filepath, "rb");
    if (!fp_in || !read_id3_header(fp_in, &old_header)) {
        printf("Error: Could not read original ID3 header.\n");
        if (fp_in) fclose(fp_in);
        return false;
    }
    fclose(fp_in);
    
    // BUILD new TIT2 frame
    uint8_t new_tit2_buffer[256];
    size_t new_tit2_size = create_tit2_frame(new_tit2_buffer, new_title);
    
    return rewrite_file_with_new_frame(filepath, &old_tag_data, &old_header, 
                                       "TIT2", new_tit2_buffer, new_tit2_size,
                                       old_tag_data.title_pos, old_tag_data.title_size_raw);
}

// Main function to edit the artist tag <--- NEW PUBLIC FUNCTION
bool edit_tag_artist(const char *filepath, const char *new_artist) {
    TagData old_tag_data;
    if (!read_tags_from_file(filepath, &old_tag_data)) {
        printf("Error: Could not read original tag data.\n");
        return false;
    }
    
    ID3Header old_header;
    FILE *fp_in = fopen(filepath, "rb");
    if (!fp_in || !read_id3_header(fp_in, &old_header)) {
        printf("Error: Could not read original ID3 header.\n");
        if (fp_in) fclose(fp_in);
        return false;
    }
    fclose(fp_in);
    
    // BUILD new TPE1 frame
    uint8_t new_tpe1_buffer[256];
    size_t new_tpe1_size = create_tpe1_frame(new_tpe1_buffer, new_artist);
    
    return rewrite_file_with_new_frame(filepath, &old_tag_data, &old_header, 
                                       "TPE1", new_tpe1_buffer, new_tpe1_size,
                                       old_tag_data.artist_pos, old_tag_data.artist_size_raw);
}