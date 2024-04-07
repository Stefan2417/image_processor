#pragma once
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <fstream>
#include "../filters/filters.h"

class Parser {
public:
    template <typename T>
    Parser& AddFilter(const T& filter) {
        fc_.InsertFilter(filter);
        return *this;
    }

    void ParseBMP(size_t argc, char* argv[]);

    void ParseOptions(size_t argc, char* argv[], BMP& bmp);

    std::ifstream& GetInputFileStream() {
        return input_file_stream_;
    }

    std::ofstream& GetOutputFileStream() {
        return output_file_stream_;
    }

    void PrintWindow();

private:
    FilterController fc_;
    std::ifstream input_file_stream_;
    std::ofstream output_file_stream_;
};