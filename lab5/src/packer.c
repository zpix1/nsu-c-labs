#include "packer.h"
#include "bitreader.h"
#include "bitwriter.h"
#include "coder.h"

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

char* strdup(const char* str) {
    int n = strlen(str) + 1;
    char* dup = malloc(n);
    if (dup != NULL) {
        strcpy(dup, str);
    }
    return dup;
}

size_t list_dir(const char* name, char** filenames, int offset) {
    DIR* dir;
    struct dirent* entry;

    if (!(dir = opendir(name))) {
        fprintf(stderr, "error: can't open dir\n");
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[MAX_FILENAME_S];
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, MAX_FILENAME_S, "%s/%s", name, entry->d_name);

            offset = list_dir(path, filenames, offset);
        } else {
            filenames[offset] = malloc(MAX_FILENAME_S);
            snprintf(filenames[offset], MAX_FILENAME_S, "%s/%s", name,
                     entry->d_name);

            offset += 1;
        }
    }
    closedir(dir);
    
    return offset;
}

int isdir(const char* path) {
    struct stat st;
    if (NULL == path) {
        return 1;
    }
    stat(path, &st);
    return S_ISDIR(st.st_mode) ? 1 : 0;
}

void rek_mkdir(char* path) {
    char* sep = strrchr(path, PATH_SEP);
    if (sep != NULL) {
        *sep = 0;
        rek_mkdir(path);
        *sep = PATH_SEP;
    }
    errno = 0;
    if (!mkdir(path, 0755) && (errno == EACCES)) {
        printf("error while trying to create '%s'\n", path);
    }
}

FILE* fopen_mkdir(char* path, char* mode) {
    char* sep = strrchr(path, PATH_SEP);
    if (sep) {
        char* path0 = strdup(path);
        path0[sep - path] = 0;
        rek_mkdir(path0);
        free(path0);
    }
    return fopen(path, mode);
}

unsigned long fsize(FILE* file) {
    unsigned long orig = (unsigned long)ftell(file);
    fseek(file, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(file);
    fseek(file, orig, SEEK_SET);
    return len;
}

// Pack files to output_filename using huffman coding according to this file
// structure: 4b (uint) - n - amount of files desc of n files:
//      4b (uint) - filename_s - filename size
//      filename_s b - filename
//      4b (uint) - file_s - file size
//      k bytes - decoding tree
//      file_s bytes - encoded data
int pack_files(char** filenames, size_t nfilenames, char* outfilename) {
    FILE* outfile = fopen(outfilename, "wb");

    if (outfile == NULL) {
        fprintf(stderr, "%s can't be created, aborting\n", outfilename);
        return 1;
    }

    fwrite(&nfilenames, sizeof(size_t), 1, outfile);

    for (size_t i = 0; i < nfilenames; i++) {
        size_t filename_s = strlen(filenames[i]);
        fwrite(&filename_s, sizeof(size_t), 1, outfile);

        fwrite(filenames[i], sizeof(unsigned char), filename_s, outfile);

        FILE* current_f = fopen(filenames[i], "rb");
        if (current_f == NULL) {
            fprintf(stderr, "%s can't be found, aborting\n", filenames[i]);
            fclose(outfile);
            remove(outfilename);
            return 1;
        }

        unsigned int file_s = fsize(current_f);

        pack_file(current_f, outfile, file_s);
        fclose(current_f);
    }

    fclose(outfile);
    return 0;
}

int unpack_files(char* infilename, char* outdir) {
    FILE* infile = fopen(infilename, "rb");
    if (infile == NULL) {
        fprintf(stderr, "%s can't be found, aborting\n", infilename);
        return 1;
    }

    rek_mkdir(outdir);

    size_t outdir_s = strlen(outdir);

    size_t nfilenames;
    size_t read = fread(&nfilenames, sizeof(size_t), 1, infile);

    if (read != 1) {
        fclose(infile);
        return 1;
    }

    printf("%zu files found in %s\n", nfilenames, infilename);

    for (size_t i = 0; i < nfilenames; i++) {
        size_t filename_s;
        size_t read = fread(&filename_s, sizeof(size_t), 1, infile);
        if (read != 1) {
            return 1;
        }

        char filename[filename_s + outdir_s + 1];
        strcpy(filename, outdir);
        filename[outdir_s] = PATH_SEP;
        read = fread(filename + outdir_s + 1, sizeof(unsigned char), filename_s,
                     infile);
        if (read != filename_s) {
            return 1;
        }
        filename[outdir_s + filename_s + 1] = '\0';

        printf("decompressing %s\n", filename);

        FILE* current_f = fopen_mkdir(filename, "wb");

        unpack_file(infile, current_f);

        fclose(current_f);
    }

    fclose(infile);

    return 0;
}

int pack_file(FILE* infile, FILE* outfile, unsigned int infile_size) {
    FreqNode* tree = generate_code_tree(infile, infile_size);

    fwrite(&infile_size, sizeof(unsigned int), 1, outfile);

    BitWriter* bw = BitWriter__new(outfile);
    write_tree(tree, bw);
    BitWriter__write_buffer(bw);
    BitWriter__destroy(bw);
    encode_data(tree, infile, infile_size, outfile);

    free_tree(tree);

    return 0;
}

int unpack_file(FILE* infile, FILE* outfile) {
    unsigned int infile_size;
    size_t read = fread(&infile_size, sizeof(unsigned int), 1, infile);
    if (read != 1) {
        return 1;
    }
    BitReader* br = BitReader__new(infile);
    FreqNode* tree = read_tree(br);
    BitReader__destroy(br);
    decode_data(tree, infile, infile_size, outfile);

    free_tree(tree);

    return 0;
}
