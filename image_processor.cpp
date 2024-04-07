#include "parser/parser.h"
#include "bmp/bmp.h"
#include "filters/filters.h"

void GenerateOptions(Parser& p) {
    Crop crop(2);
    GrayScale gs(0);
    Negative neg(0);
    Sharpening sharp(0);
    EdgeDetection edge(1);
    GaussianBlur blur(1);
    Anaglyph anaglyph(1);
    p.AddFilter(crop);
    p.AddFilter(gs);
    p.AddFilter(neg);
    p.AddFilter(sharp);
    p.AddFilter(edge);
    p.AddFilter(blur);
    p.AddFilter(anaglyph);
}

int main(int argc, char** argv) {
    Parser inp;
    GenerateOptions(inp);

    try {
        inp.ParseBMP(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        inp.PrintWindow();
        return 0;
    }
    BMP file;
    file.ReadBMP(inp.GetInputFileStream());
    try {
        inp.ParseOptions(argc, argv, file);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        inp.PrintWindow();
        return 0;
    }
    inp.GetInputFileStream().close();

    file.WriteBMP(inp.GetOutputFileStream());

    inp.GetOutputFileStream().close();
}
