#pragma once

#include <stdlib.h>
#include <stdio.h>

#define PATH_SEP '/'
#define MAX_NFILENAMES 100000
#define MAX_FILENAME_S 1000

// filenames should be freed after use
size_t list_dir(const char* name, char** filenames, int offset);

// Check if given path is a directory
int isdir(const char* path);

// Return file size of given file (only for files <= 2GB)
unsigned long fsize(FILE* file);

// Pack files to output_filename using huffman coding according to this file
// structure: 4b (uint) - n - amount of files desc of n files:
//      4b (uint) - filename_s - filename size
//      filename_s b - filename
//      4b (uint) - file_s - file size
//      k bytes - decoding tree
//      file_s bytes - encoded data
int pack_files(char** filenames, size_t nfilenames, char* outfilename);

// Unpack files from file previously created with pack_files to outdir
int unpack_files(char* infilename, char* outdir);

// Compress and write infile to outfile
int pack_file(FILE* infile, FILE* outfile, unsigned int infile_size);

// Decompress and write infile to outfile
int unpack_file(FILE* infile, FILE* outfile);
