#include "graphics.h"

const double MAX_COLOR = 1;

Pixel::Pixel(double red, double green, double blue) : red(red), green(green), blue(blue) {
}

Pixel Pixel::Red() {
    return Pixel(MAX_COLOR, 0, 0);
}

Pixel Pixel::Green() {
    return Pixel(0, MAX_COLOR, 0);
}

Pixel Pixel::Blue() {
    return Pixel(0, 0, MAX_COLOR);
}

Pixel Pixel::White() {
    return Pixel(MAX_COLOR, MAX_COLOR, MAX_COLOR);
}

Pixel Pixel::Black() {
    return Pixel(0, 0, 0);
}

Matrix::Matrix(size_t n, size_t m, const std::vector<std::vector<double>> &mat) : n(n), m(m), mat(mat) {
}

Matrix::Matrix(size_t n, size_t m) : n(n), m(m) {
    mat.resize(n, std::vector<double>(m));
}

std::vector<double> &Matrix::operator[](size_t index) {
    return mat[index];
}

const std::vector<double> &Matrix::operator[](size_t index) const {
    return mat[index];
}
