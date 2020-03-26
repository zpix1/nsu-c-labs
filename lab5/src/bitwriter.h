#pragma once

#include <stdio.h>
#include <stdlib.h>

#define BW_BUFFER_SIZE 1000

typedef struct BitWriter {
    FILE* f;
    unsigned char* buffer;
    unsigned char current_byte;
    size_t byte_offset;
    size_t buffer_offset;
} BitWriter;

// OOP Simulator

// f should be opened in binary writing mode
BitWriter* BitWriter__new(FILE* f);

// Write bit (0 or 1)
void BitWriter__write_bit(BitWriter* this, char bit);

// Write the rest of the buffer (should be called after writing)
void BitWriter__write_buffer(BitWriter* this);

// Destroy BitWriter
void BitWriter__destroy(BitWriter* this);
