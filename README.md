Overview: A command-line utility designed to extract, interpret, and modify metadata (ID3v2.3 tags) embedded within MP3 audio files.
The software parses the raw byte stream of an MP3 file to locate specific data blocks containing track informationlike Title, Artist, and Album. 
This project highlights the practical application of file I/O and data structure parsing in a systems-level environment.

Key Features
    
  Metadata Extraction: Identifies and decodes ID3 standard versions (specifically v2.3) to retrieve hidden track data.
  Tag Editing: Supports modifying existing tags, such as Title and Artist, directly within the binary file.
  Formatted Visualization: Presents extracted metadata in a clean, structured table format within the Linux terminal.
  Persistent Updates: Changes made to the metadata are saved directly to the core file without altering the audio content.
  Robust Testing: Includes built-in functionality to generate dummy MP3 files with valid ID3 headers for verification purposes.
  
Technical Implementation

  Language: C.
  Standards: ID3v2.3 tagging standard.
  Modular Architecture: Organized into specific modules for reading (read.h), editing (edit.h), and data type definitions (types.h).
  Binary Handling: Uses fopen, fwrite, and fread for precise byte-level manipulation of audio files.

How to Use
    
  Compile the Project:
  Read Metadata:
  Edit Artist or Title:
