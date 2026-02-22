#ifndef READ_H
#define READ_H

#include <stdio.h>
#include "types.h"

// --- Public Read Function ---
bool read_tags_from_file(const char *filepath, TagData *tag_data);

// --- Internal Helper Functions ---
bool read_id3_header(FILE *fp, ID3Header *header);
bool parse_next_frame(FILE *fp, uint32_t tag_end_pos, TagData *tag_data);

#endif // READ_H