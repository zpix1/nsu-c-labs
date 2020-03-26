#pragma once

#include <stdio.h>
#include <stdlib.h>

#define BR_BUFFER_SIZE 1000

typedef struct BitReader {
    FILE* f;
    unsigned char* buffer;
    unsigned char current_byte;
    size_t byte_offset;
    size_t buffer_offset;
    size_t max_buffer_size;
} BitReader;

// f should be opened in binary reading mode
BitReader* BitReader__new(FILE* f);

// Return 1 if bit is 1, 0 if 0, and -1 if bit can't be read
char BitReader__read_bit(BitReader* this);

// Destroy BitReader, not the file
void BitReader__destroy(BitReader* this);
