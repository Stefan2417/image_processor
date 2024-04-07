#pragma once
#include <array>
#include <cmath>
#include <limits>
#include <iostream>
#include <vector>
#include <fstream>
#include "../graphics/graphics.h"

struct BitMapFileHeader {
    uint16_t bf_type;
    uint32_t bf_size;
    uint16_t bf_reversed1;
    uint16_t bf_reversed2;
    uint32_t bf_offset;

    void ReadBitMapFileHeader(std::ifstream &f);
    void WriteBitMapFileHeader(std::ofstream &f);
};

struct BitMapInfoHeader {
    uint32_t bi_size;
    uint32_t bi_width;
    uint32_t bi_height;
    uint16_t bi_planes;
    uint16_t bi_bit_count;
    uint32_t bi_compression;
    uint32_t bi_image_size;
    uint32_t bi_hor_ppm;
    uint32_t bi_vert_ppm;
    uint32_t bi_colors_used;
    uint32_t bi_colors_important;
    void ReadBitMapInfoHeader(std::ifstream &f);
    void WriteBitMapInfoHeader(std::ofstream &f);
};

template <typename T>
void Read(std::ifstream &f, T &result, std::streamsize sz) {
    f.read(reinterpret_cast<char *>(&result), sz);
}

template <typename T>
void Write(std::ofstream &f, T &result, std::streamsize sz) {
    f.write(reinterpret_cast<char *>(&result), sz);
}

class BMP {
public:
    BitMapFileHeader bmp_fh;
    BitMapInfoHeader bmp_ih;
    std::vector<std::vector<Pixel>> data;
    BMP() = default;

    void ReadBMP(std::ifstream &f);
    void WriteBMP(std::ofstream &f);
};

void ApplyMatrixForBMP(BMP &bmp, const Matrix &applied_matrix, int32_t delta = 1);

void RelaxToNearest(int32_t &x, int32_t &y, int32_t h, int32_t w);