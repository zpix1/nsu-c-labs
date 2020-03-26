#pragma once

#include <stdlib.h>
#include <stdio.h>

#include "bitwriter.h"
#include "bitreader.h"

#define BYTE_COUNT 256
#define C_BUFFER_SIZE 1000

#define PRINT_FN(FN) fprintf(stderr, "%20s: freq=%5d, c='%2d',  zero=%p, one=%p (%p)\n", #FN,  FN->freq, FN->c, FN->zero_node, FN->one_node, FN);

typedef struct FreqNode {
    unsigned int freq;
    unsigned char c;

    struct FreqNode* zero_node;
    struct FreqNode* one_node;
} FreqNode;

typedef struct CharCode {
    unsigned char code_length;
    char code[255];
} CharCode;

void free_tree(FreqNode* head);

// Generate freq_tree and return the caret to the original state
FreqNode* generate_code_tree(FILE* infile, size_t size);

// Encode and write size bytes from infile to outfile
void encode_data(FreqNode* freq_tree, FILE* infile, size_t size, FILE* outfile);

// Decode and write size bytes from infile to outfile
void decode_data(FreqNode* freq_tree, FILE* infile, size_t size, FILE* outfile);

// Write FreqNode tree to file using given BitWriter
void write_tree(FreqNode* freq_tree, BitWriter* br);

// Read FreqNode tree from file using given BitReader
FreqNode* read_tree(BitReader* br);
