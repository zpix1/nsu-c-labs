#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void usage() {
    printf("usage ./diff file1 file2\n");
}

#define BUFFER_SIZE 1000

// Return size of binary file in bytes
// NOTE: resets current postion
long int get_filesize(FILE* f) {
    fseek(f, 0, SEEK_END);
    long int ans = ftell(f);
    fseek(f, 0, SEEK_SET);

    return ans;
}

int is_newline(char c) {
    if (c == '\n') {
        return 1;
    }
    return 0;
}

int my_isprint(char c) {
    if (isprint(c) || is_newline(c)) {
        return 1;
    }
    return 0;
}

// NOTE: corrupts current file position
void print_str_from_file(FILE* f, long int position) {
    fseek(f, position, SEEK_SET);
    char* buffer = (char*) malloc((BUFFER_SIZE + 1) * sizeof(char));
    int read;
    while ((read = fread(buffer, 1, BUFFER_SIZE, f)) != 0) {
        buffer[read] = '\0';
        for (int i = 0; i < read; i++) {
            if (is_newline(buffer[i])) {
                buffer[i] = '\0';
                printf("%s\n", buffer);
                free(buffer);
                return;
            }
        }
        printf("%s", buffer);
    }
    printf("\n");
    free(buffer);
}

void print_binary_str_from_file(FILE* f, long int position) {
    fseek(f, position, SEEK_SET);
    char* buffer = (char*) malloc((32) * sizeof(char));
    memset(buffer, 0, 32);
    int read = fread(buffer, 1, 32, f);
    printf("%08X   ", ((unsigned int)position & 0xFFFFFFFF));
    for (int i = 0; i < read; i++) {
        printf("%02X ", buffer[i] & 0xFF);
        if (i % 8 == 7 && i != 31) {
            printf("| ");
        }
    }
    printf("\n");
    free(buffer);
}

void print_text_diff(FILE* file1, FILE* file2, long int position) {
    print_str_from_file(file1, position);

    fseek(file1, position, SEEK_SET);
    fseek(file2, position, SEEK_SET);

    int read = 0;
    char* buffer1 = (char*) malloc(BUFFER_SIZE * sizeof(char));
    char* buffer2 = (char*) malloc(BUFFER_SIZE * sizeof(char));
    while ((read != -1) 
            && (read = fread(buffer1, 1, BUFFER_SIZE, file1)) != 0
            && (fread(buffer2, 1, BUFFER_SIZE, file2) != 0)) {
        for (int i = 0; i < read && read != -1; i++) {
            if (is_newline(buffer1[i]) || is_newline(buffer1[i])) {
                read = -1;
            }
            if (buffer1[i] != buffer2[i]) {
                printf("%c", '+');
            } else {
                printf("%c", ' ');
            }
        }
    }
    
    printf("\n");

    free(buffer1);
    free(buffer2);

    print_str_from_file(file2, position);
}

void print_binary_diff(FILE* file1, FILE* file2, long int position) {
    print_binary_str_from_file(file1, position);

    fseek(file1, position, SEEK_SET);
    fseek(file2, position, SEEK_SET);

    char* buffer1 = (char*) malloc((32) * sizeof(char));
    char* buffer2 = (char*) malloc((32) * sizeof(char));
    memset(buffer1, 0, 32);
    memset(buffer2, 0, 32);
    
    int read = fread(buffer1, 1, 32, file1);
    fread(buffer2, 1, 32, file2);
    printf("           ");
    for (int i = 0; i < read; i++) {
        int hex11 = (buffer1[i] & (0xF0)) >> 4;
        int hex12 = buffer1[i] & (0x0F);
        int hex21 = (buffer2[i] & (0xF0)) >> 4;
        int hex22 = buffer2[i] & (0x0F);
        if (hex11 != hex21) {
            printf("%s", "+");
        } else {
            printf("%s", " ");
        }
        if (hex12 != hex22) {
            printf("%s", "+");
        } else {
            printf("%s", " ");
        }
        printf("%s", " ");
        if (i % 8 == 7 && i != 31) {
            printf("| ");
        }
    }
    printf("\n");
    print_binary_str_from_file(file2, position);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("wrong amount of arguments\n");
        usage();
        exit(2);
    }
    char* filename1 = argv[1];
    char* filename2 = argv[2];

    FILE* file1 = fopen(filename1, "rb");
    if (file1 == NULL) {
        printf("%s does not exist or can't be read\n", filename1);
        usage();
        exit(2);
    }
    FILE* file2 = fopen(filename2, "rb");
    if (file2 == NULL) {
        printf("%s does not exist or can't be read\n", filename2);
        usage();
        exit(2);
    }

    long int size1 = get_filesize(file1);
    long int size2 = get_filesize(file2);
    
    if (size1 != size2) {
        printf("Size mismatch: %s is %ld bytes, %s is %ld bytes\n", filename1, size1, filename2, size2);
        exit(1);
    } 

    int is_binary_mode = 0;
    long int last_row_start_postion = 0;
    long int current_block_position = 0;

    int read = 0;
    int lineno = 1;
    char* buffer1 = (char*) malloc(BUFFER_SIZE * sizeof(char));
    char* buffer2 = (char*) malloc(BUFFER_SIZE * sizeof(char));
    while ( (read != -1)
            && (read = fread(buffer1, 1, BUFFER_SIZE, file1)) != 0
            && (fread(buffer2, 1, BUFFER_SIZE, file2) != 0)) {
        for (int i = 0; i < read; i++) {
            unsigned int current_byte = current_block_position + i;

            if (!my_isprint(buffer1[i]) || !my_isprint(buffer2[i]) ) {
                is_binary_mode = 1;
            }
            if (buffer1[i] != buffer2[i]) {
                if (!is_binary_mode) {
                    if (is_newline(buffer1[i])) {
                        printf("Discrepancy at byte %d, at line %d('\\n' vs '%c')\n", current_byte, lineno, buffer2[i]);
                    } else if (is_newline(buffer2[i])) {
                        printf("Discrepancy at byte %d, at line %d('%c' vs '\\n')\n", current_byte, lineno, buffer1[i]);
                    } else {
                        printf("Discrepancy at byte %d, at line %d('%c' vs '%c')\n", current_byte, lineno, buffer1[i], buffer2[i]);
                    }

                    print_text_diff(file1, file2, last_row_start_postion);
                } else {
                    printf("Discrepancy at byte %08X (%02X vs %02X)\n", current_byte, buffer1[i] & 0xFF, buffer2[i] & 0xFF);
                    print_binary_diff(file1, file2, current_byte - (current_byte) % 32);
                }
                
                read = -1;
                break;
            }
            if (is_newline(buffer1[i]) || is_newline(buffer2[i])) {
                lineno++;
                last_row_start_postion = current_byte + 1;
            }
        }
        current_block_position += read;
    }
    int return_code = 0;

    if (read != -1) {
        printf("OK\n");
    } else {
        return_code = 1;
    }

    free(buffer1);
    free(buffer2);

    return return_code;
}