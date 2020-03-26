#include <stdio.h>
#include <string.h>

#include "packer.h"

void usage() {
    printf("usage:\thuffman [c]ompress <archive_name> [<file/directory_names>...]\n");
    printf("\thuffman [d]ecompress <archive_name> [<output_dir_name>]\n");
    printf("\thuffman [r]ecursive <archive_name> <directory_name>\n");
}

int main(int argc, char** argv) {

    if (argc < 3) {
        fprintf(stderr, "error: not enough aruments, check usage\n");
        usage();
        return 1;
    }
    if (!strcmp(argv[1], "c") || !strcmp(argv[1], "compress")) {
        size_t nfilenames = argc - 3;
        char* outfilename = argv[2];
        printf("compressing to %s\n", outfilename);

        for (size_t i = argc - nfilenames; i < argc; i++) {
            if (isdir(argv[i])) {
                fprintf(stderr, "error: %s is a directory, to compress directories use recursive mode\n", argv[i]);
                return 0;
            }
        }

        pack_files(argv + 3, nfilenames, outfilename);

    } else if (!strcmp(argv[1], "d") || !strcmp(argv[1], "decompress")) {
        char* infilename = argv[2];
        char* outdir = argv[3];

        unpack_files(infilename, outdir);

    } else if (!strcmp(argv[1], "r") || !strcmp(argv[1], "recursive")) {
        char* outfilename = argv[2];
        char* indir = argv[3];
        
        printf("compressing recursively to %s\n", outfilename);
        char* filenames[MAX_NFILENAMES] = { 0 };
        
        size_t nfilenames = list_dir(indir, filenames, 0);

        pack_files(filenames, nfilenames, outfilename);

        for (size_t i = 0; i < MAX_NFILENAMES; i++) {
            if (filenames[i] != NULL) {
                free(filenames[i]);
            }
        }
    } else {
        printf("error: wrong mode, check usage\n");
        usage();
    }
    return 0;
}
