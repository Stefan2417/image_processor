#include "parser.h"
#include "../exceptions/exceptions.h"

void Parser::PrintWindow() {
    std::cerr << fc_.GenerateWindow();
}

void Parser::ParseBMP(size_t argc, char** argv) {
    if (argc == 1) {
        throw OptionExceptions(EMPTY_OPTIONS);
    }
    if (argc == 2) {
        throw OptionExceptions(EMPTY_OUTPUT_FILE);
    }
    char* input_file_name = argv[1];
    char* output_file_name = argv[2];
    input_file_stream_ = std::ifstream(input_file_name, std::ifstream::binary);
    if (!input_file_stream_.is_open()) {
        throw OptionExceptions(INVALID_INPUT_FILE);
    }
    output_file_stream_ = std::ofstream(output_file_name, std::ofstream::binary);
    if (!output_file_stream_.is_open()) {
        throw OptionExceptions(INVALID_OUTPUT_FILE);
    }
}

void Parser::ParseOptions(size_t argc, char** argv, BMP& bmp) {
    for (size_t i = 3; i < argc;) {
        const std::unique_ptr<Filter>& f_ptr = fc_.GiveFilterPattern(argv[i]);
        std::unique_ptr<Filter> f_clone = f_ptr->Clone();
        i = f_clone->Parse(argc, argv, i + 1);
        f_clone->Apply(bmp);
    }
}
