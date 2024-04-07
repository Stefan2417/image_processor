#include "bmp.h"
#include "../exceptions/exceptions.h"

const uint16_t BMP_FORMAT = 0x4D42;
const uint32_t BMP_BI_NEED_SIZE = 40;
const uint16_t BMP_BI_BIT_COUNT = 24;
const uint16_t BMP_BI_COMPRESSION = 0;
const uint32_t BYTE = 3;
const uint32_t ALLIGN = 4;
const double MAX_COLOR = std::numeric_limits<uint8_t>::max();

void BitMapFileHeader::ReadBitMapFileHeader(std::ifstream &f) {
    Read(f, bf_type, sizeof(bf_type));
    if (bf_type != BMP_FORMAT) {
        throw BMPExceptions(FILE_FORMAT);
    }
    Read(f, bf_size, sizeof(bf_size));
    Read(f, bf_reversed1, sizeof(bf_reversed1));
    Read(f, bf_reversed2, sizeof(bf_reversed2));
    Read(f, bf_offset, sizeof(bf_offset));
}

void BitMapFileHeader::WriteBitMapFileHeader(std::ofstream &f) {
    Write(f, bf_type, sizeof(bf_type));
    Write(f, bf_size, sizeof(bf_size));
    Write(f, bf_reversed1, sizeof(bf_reversed1));
    Write(f, bf_reversed2, sizeof(bf_reversed2));
    Write(f, bf_offset, sizeof(bf_offset));
}

void BitMapInfoHeader::ReadBitMapInfoHeader(std::ifstream &f) {
    Read(f, bi_size, sizeof(bi_size));
    if (bi_size != BMP_BI_NEED_SIZE) {
        throw BMPExceptions(HEADER_NAME);
    }
    Read(f, bi_width, sizeof(bi_width));
    Read(f, bi_height, sizeof(bi_height));
    Read(f, bi_planes, sizeof(bi_planes));
    Read(f, bi_bit_count, sizeof(bi_bit_count));
    if (this->bi_bit_count != BMP_BI_BIT_COUNT) {
        throw BMPExceptions(BIT_COUNT);
    }
    Read(f, bi_compression, sizeof(bi_compression));
    if (bi_compression != BMP_BI_COMPRESSION) {
        throw BMPExceptions(COMPRESSION);
    }
    Read(f, bi_image_size, sizeof(bi_image_size));
    Read(f, bi_hor_ppm, sizeof(bi_hor_ppm));
    Read(f, bi_vert_ppm, sizeof(bi_vert_ppm));
    Read(f, bi_colors_used, sizeof(bi_colors_used));
    Read(f, bi_colors_important, sizeof(bi_colors_important));
}

void BitMapInfoHeader::WriteBitMapInfoHeader(std::ofstream &f) {
    Write(f, bi_size, sizeof(bi_size));
    Write(f, bi_width, sizeof(bi_width));
    Write(f, bi_height, sizeof(bi_height));
    Write(f, bi_planes, sizeof(bi_planes));
    Write(f, bi_bit_count, sizeof(bi_bit_count));
    Write(f, bi_compression, sizeof(bi_compression));
    Write(f, bi_image_size, sizeof(bi_image_size));
    Write(f, bi_hor_ppm, sizeof(bi_hor_ppm));
    Write(f, bi_vert_ppm, sizeof(bi_vert_ppm));
    Write(f, bi_colors_used, sizeof(bi_colors_used));
    Write(f, bi_colors_important, sizeof(bi_colors_important));
}

uint8_t ExtractByte(uint32_t byte_id, uint32_t buffer) {
    return (buffer >> (byte_id * (1 << BYTE))) & ((1 << (1 << BYTE)) - 1);
}

void BMP::ReadBMP(std::ifstream &f) {
    bmp_fh.ReadBitMapFileHeader(f);
    bmp_ih.ReadBitMapInfoHeader(f);
    data.resize(bmp_ih.bi_height, std::vector<Pixel>(bmp_ih.bi_width));
    uint32_t padding = (ALLIGN - (bmp_ih.bi_width * (bmp_ih.bi_bit_count >> BYTE))) % ALLIGN;
    for (size_t i = bmp_ih.bi_height; i--;) {
        for (size_t j = 0; j < bmp_ih.bi_width; ++j) {
            uint32_t buffer = 0;
            Read(f, buffer, bmp_ih.bi_bit_count >> BYTE);
            data[i][j].blue = static_cast<double>(ExtractByte(0, buffer)) / MAX_COLOR;
            data[i][j].green = static_cast<double>(ExtractByte(1, buffer)) / MAX_COLOR;
            data[i][j].red = static_cast<double>(ExtractByte(2, buffer)) / MAX_COLOR;
        }
        f.seekg(padding, std::ios_base::cur);
    }
}

double NormalizeValue(double x) {
    return std::max(0.0, std::min(x, 1.0));
}

void BMP::WriteBMP(std::ofstream &f) {
    bmp_fh.bf_size = sizeof(bmp_fh) + sizeof(bmp_ih) + BYTE * bmp_ih.bi_width * bmp_ih.bi_height;
    bmp_fh.WriteBitMapFileHeader(f);
    bmp_ih.WriteBitMapInfoHeader(f);
    uint32_t padding = (ALLIGN - (bmp_ih.bi_width * (bmp_ih.bi_bit_count >> BYTE))) % ALLIGN;
    for (size_t i = bmp_ih.bi_height; i--;) {
        for (size_t j = 0; j < bmp_ih.bi_width; ++j) {
            uint8_t blue = static_cast<uint8_t>(std::round(MAX_COLOR * NormalizeValue(data[i][j].blue)));
            uint8_t green = static_cast<uint8_t>(std::round(MAX_COLOR * NormalizeValue(data[i][j].green)));
            uint8_t red = static_cast<uint8_t>(std::round(MAX_COLOR * NormalizeValue(data[i][j].red)));
            Write(f, blue, sizeof(blue));
            Write(f, green, sizeof(green));
            Write(f, red, sizeof(red));
        }
        uint32_t zero = 0;
        Write(f, zero, padding);
    }
}

// void BMP::PrintRGB() {  // for debug
//     for (uint32_t i = 0; i < bmp_ih.bi_height; ++i) {
//         for (uint32_t j = 0; j < bmp_ih.bi_width; ++j) {
//             std::cout << (uint32_t)data[i][j].red << ' ' << (uint32_t)data[i][j].green << ' '
//                       << (uint32_t)data[i][j].blue << "       ";
//         }
//         std::cout << std::endl;
//     }
//     std::cout << std::endl;
//     std::cout << std::endl;
// }

void RelaxToNearest(int32_t &x, int32_t &y, int32_t h, int32_t w) {
    if (x < 0) {
        x = 0;
    } else if (x >= h) {
        x = h - 1;
    }
    if (y < 0) {
        y = 0;
    } else if (y >= w) {
        y = w - 1;
    }
}

void ApplyMatrixForBMP(BMP &bmp, const Matrix &applied_matrix, int32_t delta) {
    std::vector<std::vector<Pixel>> old_data = bmp.data;
    int32_t height = static_cast<int32_t>(bmp.bmp_ih.bi_height);
    int32_t width = static_cast<int32_t>(bmp.bmp_ih.bi_width);
    for (int32_t i = 0; i < height; ++i) {
        for (int32_t j = 0; j < width; ++j) {
            std::array<double, 3> color_result = {0, 0, 0};
            for (int32_t delta_i = -delta; delta_i <= delta; ++delta_i) {
                for (int32_t delta_j = -delta; delta_j <= delta; ++delta_j) {
                    int32_t x = i + delta_i;
                    int32_t y = j + delta_j;
                    RelaxToNearest(x, y, height, width);
                    color_result[0] += old_data[x][y].red * applied_matrix[delta_i + delta][delta_j + delta];
                    color_result[1] += old_data[x][y].green * applied_matrix[delta_i + delta][delta_j + delta];
                    color_result[2] += old_data[x][y].blue * applied_matrix[delta_i + delta][delta_j + delta];
                }
            }
            for (size_t k = 0; k < 3; ++k) {
                color_result[k] = NormalizeValue(color_result[k]);
            }
            bmp.data[i][j].red = color_result[0];
            bmp.data[i][j].green = color_result[1];
            bmp.data[i][j].blue = color_result[2];
        }
    }
}