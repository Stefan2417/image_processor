#include <sstream>
#include <cmath>
#include "filters.h"
#include "../exceptions/exceptions.h"

const double RED_COF = 0.299;
const double GREEN_COF = 0.587;
const double BLUE_COF = 0.114;
const Matrix SHAPERING_MATRIX(3, 3, {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}});
const Matrix EDGE_DETECTION_MATRIX(3, 3, {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}});
const int32_t SIGMA_BUBEN = 3;

Filter::Filter(const std::string &name, const std::string &help, size_t args_cnt)
    : name(name), help(help), args(std::vector<std::string>(args_cnt)) {
}

Filter::Filter(const std::string &name, const std::string &help, std::vector<std::string> &args)
    : name(name), help(help), args(args) {
}
Filter::Filter(const Filter &oth) {
    name = oth.name;
    help = oth.help;
    args = oth.args;
}

Crop::Crop(size_t args_cnt) : Filter("-crop", "crop the image size", args_cnt) {
}

Crop::Crop(const Crop &oth) : Filter(oth) {
}

GrayScale::GrayScale(size_t args_cnt) : Filter("-gs", "converts the image to shades of grey", args_cnt) {
}

GrayScale::GrayScale(const GrayScale &oth) : Filter(oth) {
}

Negative::Negative(size_t args_cnt) : Filter("-neg", "converts the image to a negative", args_cnt) {
}

Negative::Negative(const Negative &oth) : Filter(oth) {
}

Sharpening::Sharpening(size_t args_cnt) : Filter("-sharp", "sharpness enhancement", args_cnt) {
}

Sharpening::Sharpening(const Sharpening &sharp) : Filter(sharp) {
}

EdgeDetection::EdgeDetection(size_t args_cnt) : Filter("-edge", "edge detection", args_cnt) {
}

EdgeDetection::EdgeDetection(const EdgeDetection &edge_detection) : Filter(edge_detection) {
}

GaussianBlur::GaussianBlur(size_t args_cnt) : Filter("-blur", "gaussian blur", args_cnt) {
}

GaussianBlur::GaussianBlur(const GaussianBlur &gaussian_blur) : Filter(gaussian_blur) {
}

Anaglyph::Anaglyph(size_t args_cnt) : Filter("-anaglyph", "3d anaglyph", args_cnt) {
}

Anaglyph::Anaglyph(const Anaglyph &anaglyph) : Filter(anaglyph) {
}

void Crop::Apply(BMP &bmp) {
    if (this->args.size() != 2) {
        throw OptionExceptions(INVALID_OPTIONS);
    }
    uint32_t nwidth = std::stoul(args[0]);
    uint32_t nheight = std::stoul(args[1]);
    bmp.bmp_ih.bi_height = std::min(bmp.bmp_ih.bi_height, nheight);
    bmp.bmp_ih.bi_width = std::min(bmp.bmp_ih.bi_width, nwidth);
}

void GrayScale::Apply(BMP &bmp) {
    if (!this->args.empty()) {
        throw OptionExceptions(INVALID_OPTIONS);
    }
    for (size_t i = 0; i < bmp.bmp_ih.bi_height; ++i) {
        for (size_t j = 0; j < bmp.bmp_ih.bi_width; ++j) {
            double color =
                RED_COF * bmp.data[i][j].red + GREEN_COF * bmp.data[i][j].green + BLUE_COF * bmp.data[i][j].blue;
            bmp.data[i][j].red = color;
            bmp.data[i][j].green = color;
            bmp.data[i][j].blue = color;
        }
    }
}

void Negative::Apply(BMP &bmp) {
    if (!this->args.empty()) {
        throw OptionExceptions(INVALID_OPTIONS);
    }
    for (size_t i = 0; i < bmp.bmp_ih.bi_height; ++i) {
        for (size_t j = 0; j < bmp.bmp_ih.bi_width; ++j) {
            bmp.data[i][j].red = 1 - bmp.data[i][j].red;
            bmp.data[i][j].green = 1 - bmp.data[i][j].green;
            bmp.data[i][j].blue = 1 - bmp.data[i][j].blue;
        }
    }
}

void Sharpening::Apply(BMP &bmp) {
    if (!this->args.empty()) {
        throw OptionExceptions(INVALID_OPTIONS);
    }
    ApplyMatrixForBMP(bmp, SHAPERING_MATRIX);
}

void EdgeDetection::Apply(BMP &bmp) {
    if (this->args.size() != 1) {
        throw OptionExceptions(INVALID_OPTIONS);
    }
    GrayScale gs(0);
    gs.Apply(bmp);
    ApplyMatrixForBMP(bmp, EDGE_DETECTION_MATRIX);
    double threshold = std::stod(args[0]);
    for (size_t i = 0; i < bmp.bmp_ih.bi_height; ++i) {
        for (size_t j = 0; j < bmp.bmp_ih.bi_width; ++j) {
            if (bmp.data[i][j].red > threshold) {
                bmp.data[i][j] = Pixel::White();
            } else {
                bmp.data[i][j] = Pixel::Black();
            }
        }
    }
}

double Calc(double color, double sigma, int32_t len) {
    return color / std::exp(static_cast<double>(len * len) / (2 * sigma * sigma));
}

void GaussianBlur::Apply(BMP &bmp) {
    if (this->args.size() != 1) {
        throw OptionExceptions(INVALID_OPTIONS);
    }
    double sigma = std::stod(args[0]);
    const int32_t delta = static_cast<int32_t>(std::round(SIGMA_BUBEN * sigma));
    std::vector<std::vector<Pixel>> tmp_data = bmp.data;

    const double cof = (M_PI * 2 * sigma * sigma);
    int32_t height = static_cast<int32_t>(bmp.bmp_ih.bi_height);
    int32_t width = static_cast<int32_t>(bmp.bmp_ih.bi_width);
    for (int32_t i = 0; i < height; ++i) {
        for (int32_t j = 0; j < width; ++j) {
            double red_res = 0;
            double green_res = 0;
            double blue_res = 0;
            for (int32_t x = i - delta; x < i + delta; ++x) {
                int32_t x_nearest = x;
                RelaxToNearest(x_nearest, j, height, width);
                red_res += Calc(bmp.data[x_nearest][j].red, sigma, x - i);
                green_res += Calc(bmp.data[x_nearest][j].green, sigma, x - i);
                blue_res += Calc(bmp.data[x_nearest][j].blue, sigma, x - i);
            }
            tmp_data[i][j].red = red_res;
            tmp_data[i][j].green = green_res;
            tmp_data[i][j].blue = blue_res;
        }
    }

    for (int32_t i = 0; i < height; ++i) {
        for (int32_t j = 0; j < width; ++j) {
            double red_res = 0;
            double green_res = 0;
            double blue_res = 0;
            for (int32_t y = j - delta; y < j + delta; ++y) {
                int32_t y_nearest = y;
                RelaxToNearest(i, y_nearest, height, width);
                red_res += Calc(tmp_data[i][y_nearest].red, sigma, y - j);
                green_res += Calc(tmp_data[i][y_nearest].green, sigma, y - j);
                blue_res += Calc(tmp_data[i][y_nearest].blue, sigma, y - j);
            }
            bmp.data[i][j].red = red_res / cof;
            bmp.data[i][j].green = green_res / cof;
            bmp.data[i][j].blue = blue_res / cof;
        }
    }
}

size_t Filter::Parse(size_t argc, char *argv[], size_t i) {
    if (i + args.size() >= argc + 1) {
        throw OptionExceptions(INVALID_OPTIONS);
    }
    for (size_t j = i; j < i + args.size(); ++j) {
        std::stringstream ss(argv[j]);
        ss >> args[j - i];
    }
    return i + args.size();
}

void Anaglyph::Apply(BMP &bmp) {
    if (this->args.size() != 1) {
        throw OptionExceptions(INVALID_OPTIONS);
    }
    double offset = std::stod(args[0]);
    int32_t height = static_cast<int32_t>(bmp.bmp_ih.bi_height);
    int32_t width = static_cast<int32_t>(bmp.bmp_ih.bi_width);
    int32_t step = std::floor(offset * width);
    std::vector<std::vector<Pixel>> tmp_data = bmp.data;
    std::vector<std::vector<double>> cnt_red(height, std::vector<double>(width, 1));
    std::vector<std::vector<double>> cnt_green(height, std::vector<double>(width, 1));
    std::vector<std::vector<double>> cnt_blue(height, std::vector<double>(width, 1));
    const double cof = 2.3;
    for (int32_t i = 0; i < height; ++i) {
        for (int32_t j = 0; j < width; ++j) {
            if (j + step < width) {
                tmp_data[i][j + step].red += cof * bmp.data[i][j].red;
                cnt_red[i][j + step] += cof;
            } else {
                tmp_data[i][width - j - 1].red += bmp.data[i][width - j - 1].red;
                cnt_red[i][width - j - 1]++;
            }
            if (j - step >= 0) {
                tmp_data[i][j - step].blue += cof * bmp.data[i][j].blue;
                cnt_blue[i][j - step] += cof;
            } else {
                tmp_data[i][width - j - 1].blue += bmp.data[i][width - j - 1].blue;
                cnt_blue[i][width - j - 1]++;
            }
        }
    }
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            bmp.data[i][j].red = tmp_data[i][j].red / cnt_red[i][j];
            bmp.data[i][j].green = tmp_data[i][j].green / cnt_green[i][j];
            bmp.data[i][j].blue = tmp_data[i][j].blue / cnt_blue[i][j];
        }
    }
}

std::string FilterController::GenerateWindow() {
    std::string res;
    for (const auto &f : filter_options_) {
        res += f->name + ",    " + f->help + "\n";
    }
    return res;
}

const std::unique_ptr<Filter> &FilterController::GiveFilterPattern(const std::string &name) {
    for (const auto &f : filter_options_) {
        if (f->name == name) {
            return f;
        }
    }
    throw OptionExceptions(INVALID_OPTIONS);
}
