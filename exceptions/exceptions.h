#include <iostream>

const std::string INVALID_OPTIONS = "invalid options";
const std::string HEADER_NAME = "header name is not BitMapInfoHeader";
const std::string BIT_COUNT = "unsupported BMP bit count";
const std::string COMPRESSION = "unsupported BMP compression";
const std::string FILE_FORMAT = "wrong file format";
const std::string EMPTY_OPTIONS = "empty options";
const std::string EMPTY_OUTPUT_FILE = "empty output file";
const std::string INVALID_OUTPUT_FILE = "invalid output file";
const std::string INVALID_INPUT_FILE = "invalid input file";

class OptionExceptions : public std::runtime_error {
public:
    explicit OptionExceptions(const std::string &message) : std::runtime_error(message) {
    }
};

class BMPExceptions : public std::runtime_error {
public:
    explicit BMPExceptions(const std::string &message) : std::runtime_error(message) {
    }
};
