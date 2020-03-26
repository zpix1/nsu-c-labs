#include <string.h>

#include "coder.h"
#include "pq.h"

int min(int a, int b) {
    return a < b ? a : b;
}

FreqNode* generate_code_tree(FILE* infile, size_t size) {
    FreqNode** arr = malloc(BYTE_COUNT * sizeof(FreqNode*));
    
    for (size_t i = 0; i < BYTE_COUNT; i++) {
        arr[i] = malloc(sizeof(FreqNode));
        arr[i]->freq = 0;
        arr[i]->c = (unsigned char)i;
        arr[i]->zero_node = NULL;
        arr[i]->one_node = NULL;
    }

    int to_read = size;
    size_t read = 0;
    unsigned char* buffer = malloc(C_BUFFER_SIZE * sizeof(char));

    while ((read = fread(buffer, sizeof(char), min(to_read, C_BUFFER_SIZE), infile))) {
        for (size_t i = 0; i < read; i++) {
            arr[buffer[i]]->freq++;
        }
        to_read -= read;
    }

    // In case size is bigger than actual file size
    fseek(infile, (to_read - size), SEEK_CUR);

    // To count different nodes
    unsigned int counter = 0;

    heap_t *h = calloc(1, sizeof(heap_t));

    for (size_t i = 0; i < BYTE_COUNT; i++) {
        if (arr[i]->freq != 0) {
            counter++;
            push(h, arr[i]->freq, arr[i]);
        } else {
            free(arr[i]);
        }
    }

    FreqNode *ans;

    // Special case of one element
    if (counter == 1) {
        ans = pop(h);
    } else {
        while (h->len != 1) {
            FreqNode* new_node = malloc(sizeof(FreqNode));
            new_node->zero_node = pop(h);
            new_node->one_node = pop(h);
            new_node->freq = new_node->one_node->freq + new_node->zero_node->freq;
            new_node->c = 0;
            push(h, new_node->freq, new_node);
        }
        ans = pop(h);
    }

    free(arr);
    free(buffer);
    free(h->nodes);
    free(h);

    return ans;
}

void free_tree(FreqNode* head) {
    if (head != NULL) {
        free_tree(head->one_node);
        free_tree(head->zero_node);
        free(head);
    }
}

void fill_code_table(CharCode* code_table, FreqNode* node, CharCode current_code) {
    if (node->one_node != NULL) {
        CharCode c = current_code;
        c.code[(size_t)c.code_length] = '1';
        c.code_length = current_code.code_length + 1;
        fill_code_table(code_table, node->one_node, c);
    }
    if (node->zero_node != NULL) {
        CharCode c = current_code;
        c.code[(size_t)c.code_length] = '0';
        c.code_length = current_code.code_length + 1;
        fill_code_table(code_table, node->zero_node, c);
    }
    if (node->one_node == NULL) {
        code_table[node->c] = current_code;
        memcpy(code_table[node->c].code, current_code.code, current_code.code_length);
    }
}

void encode_data(FreqNode* freq_tree, FILE* infile, size_t size, FILE* outfile) {
    CharCode* code_table = malloc(sizeof(CharCode) * BYTE_COUNT);
    for (int i = 0; i < BYTE_COUNT; i++) {
        code_table[i].code_length = 0;
    }
    CharCode c = { .code_length = 0 };
    fill_code_table(code_table, freq_tree, c);

    unsigned char* buffer = malloc(sizeof(char) * C_BUFFER_SIZE);
    int to_read = size;
    size_t read = 0;

    BitWriter* bw = BitWriter__new(outfile);
    while ((read = fread(buffer, sizeof(char), min(to_read, C_BUFFER_SIZE), infile))) {
        for (size_t byte_i = 0; byte_i < read; byte_i++) {
            unsigned char byte = buffer[byte_i];
            for (size_t bit_i = 0; bit_i < code_table[byte].code_length; bit_i++) {
                BitWriter__write_bit(bw, code_table[byte].code[bit_i] & 1);
            }
        }
        to_read -= read;
    }

    BitWriter__write_buffer(bw);
    BitWriter__destroy(bw);

    free(buffer);
    free(code_table);
}

void decode_data(FreqNode* freq_tree, FILE* infile, size_t size, FILE* outfile) {
    unsigned char* buffer = malloc(sizeof(char) * C_BUFFER_SIZE);
    int to_write = size;

    FreqNode* current_node = freq_tree;
    if (current_node->one_node == NULL) {
        memset(buffer, current_node->c, C_BUFFER_SIZE);
        while (to_write > 0) {
            fwrite(buffer, sizeof(char), min(to_write, C_BUFFER_SIZE), outfile);
            to_write -= C_BUFFER_SIZE;
        }
    } else {
        size_t buffer_i = 0;
        size_t counter = 0;
        BitReader* br = BitReader__new(infile);
        while (counter < size) {
            char bit = BitReader__read_bit(br);
            if (bit == 0) {
                current_node = current_node->zero_node;
            } else if (bit == 1) {
                current_node = current_node->one_node;
            }
            if (bit == -1) {
                break;
            }
            if (current_node->one_node == NULL) {
                buffer[buffer_i] = current_node->c;
                current_node = freq_tree;
                counter++;
                buffer_i++;
                if (buffer_i == (size_t)min(to_write, C_BUFFER_SIZE)) {
                    to_write -= buffer_i;
                    fwrite(buffer, sizeof(char), buffer_i, outfile);
                    buffer_i = 0;
                }
            }
        }

        if (buffer_i != 0) {
            fwrite(buffer, sizeof(char), min(buffer_i, to_write), outfile);
        }
        BitReader__destroy(br);
    }
    free(buffer);
}

void write_tree(FreqNode* freq_tree, BitWriter* bw) {
    if (freq_tree == NULL) {
        return;
    }

    char flag = (freq_tree->one_node == NULL ? 1 : 0);

    BitWriter__write_bit(bw, flag & 1);
    
    if (flag == 1) {
        for (int i = 0; i < 8; i++) {
            BitWriter__write_bit(bw, (freq_tree->c >> (7 - i)) & 1);
        }
    }

    write_tree(freq_tree->zero_node, bw);
    write_tree(freq_tree->one_node, bw);
}

FreqNode* read_tree(BitReader* br) {    
    char value = 0;
    char flag;

    flag = BitReader__read_bit(br);

    if (flag == -1) {
        return NULL;
    }

    if (flag == 1) {
        for (unsigned char i = 0; i < 8; i++) {
            value |= BitReader__read_bit(br) << (8 - i - 1);
        }
    }

    FreqNode *root = malloc(sizeof(FreqNode));
    root->freq = 0;
    root->c = value;
    if (flag != 1) {
        root->zero_node = read_tree(br);
        root->one_node = read_tree(br);
    } else {
        root->zero_node = NULL;
        root->one_node = NULL;
    }
    return root;
}
