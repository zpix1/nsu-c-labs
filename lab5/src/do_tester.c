#include <stdio.h>

#include "packer.h"

int main () {
    char mode;
    FILE* infile = fopen("in.txt", "rb");
    FILE* outfile = fopen("out.txt", "wb");
    
    if (fscanf(infile, "%c\r\n", &mode) != 1) {
        fclose(infile);
        fclose(outfile);
        return 1;
    }
    int fs = fsize(infile) - 3;
    fseek(infile, 3, SEEK_SET);
    if (fs != 0) {
        if (mode == 'c') {
            pack_file(infile, outfile, fs);
        } else {
            unpack_file(infile, outfile);
        }
    }
    fclose(infile);
    fclose(outfile);
    return 0;
}
