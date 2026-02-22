#ifndef EDIT_H
#define EDIT_H

#include "types.h"

// --- Public Edit Functions ---
bool edit_tag_title(const char *filepath, const char *new_title);
bool edit_tag_artist(const char *filepath, const char *new_artist); // <--- NEW DECLARATION

#endif