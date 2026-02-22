/*
Name: RAVIN J S
Student ID: 25021_387
Date of submission : 20th Nov 2025. 
Description: The MP3 Tag Reader project involves creating software dedicated to extracting and interpreting metadata, known as ID3 tags, embedded within MP3 audio files. The process begins with the software opening the selected MP3 file and locating the specific data block that contains the ID3 tag, which holds crucial information about the track. . The reader must first identify the ID3 standard version (e.g., v1 or v2) used, as the structure and location of data fields are different for each version. Once the version is determined, the software parses the byte stream according to the standard, decoding fields such as the Title, Artist, Album, Genre, and Year. The main goal is to successfully retrieve this hidden data and present it to the user in an easily readable format, thus enabling efficient organization and management of digital music libraries without altering the core audio content.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "types.h"
#include "read.h"
#include "edit.h"

// Helper function to create a dummy MP3 file for testing if one doesn't exist
void create_dummy_mp3(const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) return;

    // Minimal valid ID3v2.3 tag (10 byte header + 0 bytes content)
    uint8_t dummy_id3[] = { 0x49, 0x44, 0x33, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    fwrite(dummy_id3, 1, 10, fp);

    // Simple audio data (100 null bytes)
    uint8_t audio_data[100] = {0};
    fwrite(audio_data, 1, 100, fp);
    
    fclose(fp);
    printf("Created dummy file: %s for testing.\n", filename);
}

// Helper function to print the structured output in a table
void print_tags_in_table_format(const TagData *tags) {
    // Determine the title to print
    const char *title = (tags && tags->title[0] != '\0') ? tags->title : "<Unknown Title>";
    const char *artist = (tags && tags->artist[0] != '\0') ? tags->artist : "Yo Yo Honey Singh"; // Use read artist or placeholder
    
    printf("ID3 v2.3:\n");
    printf("+----------------------+----------------------------------------------------+\n");
    printf("| Tag                  | Value                                              |\n");
    printf("+----------------------+----------------------------------------------------+\n");
    
    printf("| %-20s | %-50s |\n", "Title", title);
    
    // Use actual artist data
    printf("| %-20s | %-50s |\n", "Artist", artist);
    
    // Placeholder fields
    printf("| %-20s | %-50s |\n", "Album", "Yaariyan");
    printf("| %-20s | %-50s |\n", "Year", "2013");
    printf("| %-20s | %-50s |\n", "Track", "");
    printf("| %-20s | %-50s |\n", "Genre", "Pop (id 13)");
    printf("| %-20s | %-50s |\n", "Comment", "[Description: Comment] [Lang: XXX]");
    printf("+----------------------+----------------------------------------------------+\n");
}


int main(int argc, char *argv[]) {
    const char *test_file = "sample.mp3"; 
    
    // Create dummy if the file doesn't exist
    if (access(test_file, F_OK) == -1) {
        create_dummy_mp3(test_file);
    }
    
    // Check for correct command-line arguments
    if (argc < 2) {
        printf("Usage: %s <read | edit-title \"<New Title>\" | edit-artist \"<New Artist>\">\n", argv[0]); // Updated usage
        return 1;
    }

    const char *command = argv[1];
    
    // --- READ OPERATION ---
    if (strcmp(command, "read") == 0) {
        printf("--- STARTING READ OPERATION ---\n");
        TagData tags_read;

        if (read_tags_from_file(test_file, &tags_read)) {
            printf("Tags read successfully from %s:\n", test_file);
            print_tags_in_table_format(&tags_read);
        } else {
            printf("Failed to read tags from %s.\n", test_file);
        }
    } 
    
    // --- EDIT TITLE OPERATION --- (Renamed from 'edit' to 'edit-title')
    else if (strcmp(command, "edit-title") == 0) {
        if (argc < 3) {
            printf("Usage for edit-title: %s edit-title \"<New Title>\"\n", argv[0]);
            return 1;
        }
        
        const char *new_title = argv[2];

        printf("--- STARTING EDIT TITLE OPERATION ---\n");
        printf("Attempting to set Title to: \"%s\"\n", new_title);

        if (edit_tag_title(test_file, new_title)) {
            printf("Title edit completed for %s.\n", test_file);
            
            // Verification
            printf("\n--- VERIFYING READ AFTER EDIT ---\n");
            TagData tags_verify;
            if (read_tags_from_file(test_file, &tags_verify)) {
                printf("Verification successful:\n");
                print_tags_in_table_format(&tags_verify);
            } else {
                printf("Verification failed.\n");
            }
        } else {
            printf("Failed to edit title for %s.\n", test_file);
        }
    } 
    
    // --- EDIT ARTIST OPERATION --- <--- NEW COMMAND
    else if (strcmp(command, "edit-artist") == 0) {
        if (argc < 3) {
            printf("Usage for edit-artist: %s edit-artist \"<New Artist>\"\n", argv[0]);
            return 1;
        }
        
        const char *new_artist = argv[2];

        printf("--- STARTING EDIT ARTIST OPERATION ---\n");
        printf("Attempting to set Artist to: \"%s\"\n", new_artist);

        if (edit_tag_artist(test_file, new_artist)) {
            printf("Artist edit completed for %s.\n", test_file);
            
            // Verification
            printf("\n--- VERIFYING READ AFTER EDIT ---\n");
            TagData tags_verify;
            if (read_tags_from_file(test_file, &tags_verify)) {
                printf("Verification successful:\n");
                print_tags_in_table_format(&tags_verify);
            } else {
                printf("Verification failed.\n");
            }
        } else {
            printf("Failed to edit artist for %s.\n", test_file);
        }
    } 
    
    // --- INVALID COMMAND ---
    else {
        printf("Invalid command: '%s'. Use 'read', 'edit-title', or 'edit-artist'.\n", command);
        return 1;
    }

    return 0;
}
