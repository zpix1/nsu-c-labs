#include "bitwriter.h"

BitWriter* BitWriter__new(FILE* f) {
    BitWriter* bw = malloc(sizeof(BitWriter));
    bw->f = f;
    bw->buffer = malloc(sizeof(char) * BW_BUFFER_SIZE);
    bw->current_byte = 0;
    bw->byte_offset = 0;
    bw->buffer_offset = 0;
    return bw;
}

void BitWriter__write_bit(BitWriter* this, char bit) {
    this->current_byte = (this->current_byte << 1) + (bit & 1);
    this->byte_offset++;
    if (this->byte_offset == 8) {
        this->buffer[this->buffer_offset] = this->current_byte;
        this->buffer_offset++;

        this->byte_offset = 0;
        this->current_byte = 0;

        if (this->buffer_offset == BW_BUFFER_SIZE) {
            fwrite(this->buffer, sizeof(char), BW_BUFFER_SIZE, this->f);
            this->buffer_offset = 0;
        }
    }
}

void BitWriter__write_buffer(BitWriter* this) {
    if (this->byte_offset != 8) {
        this->current_byte = this->current_byte << (8 - this->byte_offset);
    }
    if (this->byte_offset != 0) {
        this->buffer[this->buffer_offset] = this->current_byte;
        this->buffer_offset++;
    }
    fwrite(this->buffer, sizeof(char), this->buffer_offset, this->f);
    this->buffer_offset = 0;
    this->current_byte = 0;
    this->byte_offset = 0;
}

void BitWriter__destroy(BitWriter* this) {
    free(this->buffer);
    free(this);
}
