#pragma once
#include <iostream>
#include <vector>

class Pixel {
public:
    double red;
    double green;
    double blue;

    Pixel() = default;
    Pixel(double red, double green, double blue);
    ~Pixel() = default;

    static Pixel Red();
    static Pixel Green();
    static Pixel Blue();
    static Pixel White();
    static Pixel Black();
};

struct Matrix {
    size_t n;
    size_t m;
    std::vector<std::vector<double>> mat;
    Matrix() = default;
    ~Matrix() = default;
    Matrix(size_t n, size_t m);
    Matrix(size_t n, size_t m, const std::vector<std::vector<double>>& mat);
    std::vector<double>& operator[](size_t index);
    const std::vector<double>& operator[](size_t index) const;
};
