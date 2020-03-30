#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Pixel {
    unsigned char r, g, b;
};

struct Image {
    struct Pixel* data;
    int width;
    int height;
};

void free_image(struct Image* image) {
    free(image->data);
    free(image);
}

int read_bmp(FILE* f, struct Image** image_ptr) {
    char signature[2];
    fread(signature, sizeof(char), 2, f);
    if (strcmp(signature, "BM")) {
        return 1;
    }

    int filesize;
    fread(&filesize, sizeof(int), 1, f);

    int unused;
    fread(&unused, sizeof(int), 1, f);

    int data_offset;
    fread(&data_offset, sizeof(int), 1, f);

    printf("filesize: %d\ndata_offset: %d\n", filesize, data_offset);


    size_t info_header_size;
    fread(&info_header_size, sizeof(int), 1, f);

    int width, height;
    fread(&width, sizeof(int), 1, f);
    fread(&height, sizeof(int), 1, f);

    printf("width: %d\nheight: %d\n", width, height);

    short planes;
    fread(&planes, sizeof(short), 1, f);
    printf("planes: %hi\n", planes);

    short bps;
    fread(&bps, sizeof(short), 1, f);
    printf("bps: %hi\n", bps);

    if (bps != 24) {
        return 2;
    }

    int compression;
    fread(&compression, sizeof(int), 1, f);
    printf("compression: %hi\n", compression);
    if (compression != 0) {
        return 3;
    }

    int c_filesize;
    fread(&c_filesize, sizeof(int), 1, f);

    int xpm;
    fread(&xpm, sizeof(int), 1, f);
    int ypm;
    fread(&ypm, sizeof(int), 1, f);

    int colors_used;
    fread(&colors_used, sizeof(int), 1, f);
    printf("colors_user: %d\n", colors_used);

    int important_colors;
    fread(&important_colors, sizeof(int), 1, f);
    printf("important_colors: %d\n", important_colors);

    // Somehow header can be more than 40 bytes o_O
    fseek(f, data_offset, SEEK_SET);

    char* data = malloc(height * width * 3);

    fread(data, sizeof(char), height * width * 3, f);

    struct Pixel* image_data = malloc(height * width * sizeof(struct Pixel));
    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            image_data[i * width + j].r = data[(height - i - 1) * width * 3 + j * 3 + 0];
            image_data[i * width + j].g = data[(height - i - 1) * width * 3 + j * 3 + 1];
            image_data[i * width + j].b = data[(height - i - 1) * width * 3 + j * 3 + 2];
        }
    }

    struct Image* image = malloc(sizeof(struct Image));

    image->data = image_data;
    image->width = width;
    image->height = height;

    *image_ptr = image;

    free(data);

    return 0;
}

int write_bmp(FILE* f, struct Image* image) {
    int width = image->width;
    int height = image->height;

    char signature[2] = "BM";
    fwrite(signature, sizeof(char), 2, f);

    int data_offset = 2 + 4 + 4 + 4 + 4 + 4 + 4 + 2 + 2 + 4 + 4 + 4 + 4 + 4 + 4;
    int filesize = data_offset + width * height * 3 + width * height * 3 % 4;

    fwrite(&filesize, sizeof(int), 1, f);
    int unused = 0;
    fwrite(&unused, sizeof(int), 1, f);

    fwrite(&data_offset, sizeof(int), 1, f);

    size_t info_header_size = 40;
    fwrite(&info_header_size, sizeof(int), 1, f);

    fwrite(&width, sizeof(int), 1, f);
    fwrite(&height, sizeof(int), 1, f);

    short planes = 1;
    fwrite(&planes, sizeof(short), 1, f);

    short bps = 24;
    fwrite(&bps, sizeof(short), 1, f);

    int compression = 0;
    fwrite(&compression, sizeof(int), 1, f);
    if (compression != 0) {
        return 3;
    }

    int c_filesize = 0;
    fwrite(&c_filesize, sizeof(int), 1, f);

    int xpm = 0;
    fwrite(&xpm, sizeof(int), 1, f);
    int ymp = 0;
    fwrite(&ymp, sizeof(int), 1, f);
    int colors_used = 256;
    fwrite(&colors_used, sizeof(int), 1, f);
    int important_colors = 0;
    fwrite(&important_colors, sizeof(int), 1, f);

    for (int i = height - 1; i >= 0; i--) {
        for (int j = 0; j < width; j++) {
            fwrite(&image->data[i * width + j].r, sizeof(char), 1, f);
            fwrite(&image->data[i * width + j].g, sizeof(char), 1, f);
            fwrite(&image->data[i * width + j].b, sizeof(char), 1, f);
        }
    }

    char zero = 0;
    for (int i = 0; i < (width * height * 3) % 4; i++) {
        fwrite(&zero, sizeof(char), 1, f);
    }

    return 0;
}
// https://www.mathworks.com/help/matlab/ref/rgb2gray.html
int convert_to_grayscale(struct Image* image) {
    for (int i = 0; i < image->height; i++) {
        for (int j = 0; j < image->width; j++) {
            float r, g, b;
            r = image->data[i * image->width + j].r / 255.0;
            g = image->data[i * image->width + j].g / 255.0;
            b = image->data[i * image->width + j].b / 255.0;
            int y = (0.2989 * r + 0.5870 * g + 0.1140 * b) * 255;
            image->data[i * image->width + j].r = image->data[i * image->width + j].g = image->data[i * image->width + j].b = y;
        }
    }
    return 0;
}

char get_byte(struct Image* image, int i) {
    switch (i % 3) {
        case 0:
            return image->data[i / 3].r;
        case 1:
            return image->data[i / 3].g;
        case 2:
            return image->data[i / 3].b;
    }
}

void set_byte(struct Image* image, int i, char byte) {
    switch (i % 3) {
        case 0:
            image->data[i / 3].r = byte;
            break;
        case 1:
            image->data[i / 3].g = byte;
            break;
        case 2:
            image->data[i / 3].b = byte;
            break;
    }
}

int lsb_encode(struct Image* image, char* str) {
    int str_length = strlen(str) + 1;
    if (str_length > (image->width * image->height * 3) / 8) {
        return 1;
    }
    for (int i = 0; i < str_length; i++) {
        for (int j = 7; j >= 0; j--) {
            char byte = get_byte(image, i * 8 + j);
            byte -= byte % 2;
            set_byte(image, i * 8 + j, byte | (str[i] & (1 << (j))) >> (j));
        }
    }
    return 0;
}

int lsb_decode(struct Image* image, char** str_ptr) {
    char* str = malloc(sizeof(char) * (image->width * image->height * 3) / 8);
    for (int i = 0; i < (image->width * image->height * 3) / 8; i++) {
        str[i] = 0;
        for (int j = 7; j >= 0; j--) {
            str[i] |= (get_byte(image, i * 8 + j) & 1) << (j);
        }
        if (str[i] == '\0') {
            break;
        }
    }
    *str_ptr = str;
    return 0;
}

int make_blur(struct Image* image) {
    struct Pixel* old_image = malloc(image->height * image->width * sizeof(struct Pixel));
    memcpy(old_image, image->data, image->height * image->width * sizeof(struct Pixel));
    
    const int kernel_size = 3;
    int offset = kernel_size / 2;
    // float kernel[5][5] =    {{1. / 256, 4.  / 256,  6. / 256,  4. / 256, 1. / 256},
    //                         {4. / 256, 16. / 256, 24. / 256, 16. / 256, 4. / 256},
    //                         {6. / 256, 24. / 256, 36. / 256, 24. / 256, 6. / 256},
    //                         {4. / 256, 16. / 256, 24. / 256, 16. / 256, 4. / 256},
    //                         {1. / 256, 4.  / 256,  6. / 256,  4. / 256, 1. / 256}};

    float kernel[3][3] ={{1 / 9., 1 / 9., 1 / 9.},
              {1 / 9., 1 / 9., 1 / 9.},
              {1 / 9., 1 / 9., 1 / 9.}};

    for (int x = offset; x < image->width - offset; x++) {
        for (int y = offset; y < image->height - offset; y++) {
            struct Pixel acc = {.r = 0, .b = 0, .g = 0};
            for (int a = 0; a < kernel_size; a++) {
                for (int b = 0; b < kernel_size; b++) {
                    int xn = x + a - offset;
                    int yn = y + b - offset;
                    struct Pixel p = old_image[yn * image->width + xn];
                    acc.r += (p.r) * kernel[a][b];
                    acc.g += (p.g) * kernel[a][b];
                    acc.b += (p.b) * kernel[a][b];
                }
            }
            image->data[y * image->width + x] = acc;
        }
    }

    free(old_image);

    return 0;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("very bad\n");
        return 1;
    }

    FILE* f = fopen(argv[1], "rb");


    int width, height;
    struct Image* image;

    if (!f || read_bmp(f, &image) != 0) {
        printf("not supported or can't be read\n");
        fclose(f);
        return 1;
    }
    fclose(f);

    // 1
    convert_to_grayscale(image);

    // 2-3
    lsb_encode(image, "lulkw");
    char* ans;
    lsb_decode(image, &ans);
    printf("lsb decoded string: %s\n", ans);
    free(ans);

    // // 4
    make_blur(image);

    FILE* f1 = fopen(argv[2], "wb");
    write_bmp(f1, image);
    fclose(f1);

    free_image(image);
    return 0;
}