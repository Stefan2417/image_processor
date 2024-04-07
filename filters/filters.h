#pragma once
#include <memory>
#include <unordered_map>
#include <vector>
#include "../bmp/bmp.h"

class Filter {
public:
    std::string name;
    std::string help;
    std::vector<std::string> args;
    Filter(const std::string &name, const std::string &help, size_t args_cnt);
    Filter(const std::string &name, const std::string &help, std::vector<std::string> &args);
    Filter(const Filter &oth);
    size_t Parse(size_t argc, char *argv[], size_t i);
    virtual ~Filter() = default;
    virtual void Apply(BMP &bmp) = 0;
    virtual std::unique_ptr<Filter> Clone() const = 0;
};

class Crop : public Filter {
public:
    explicit Crop(size_t args_cnt);
    Crop(const Crop &cr);
    ~Crop() override = default;
    void Apply(BMP &bmp) override;
    std::unique_ptr<Filter> Clone() const override {
        return std::make_unique<Crop>(*this);
    }
};

class GrayScale : public Filter {
public:
    explicit GrayScale(size_t args_cnt);
    GrayScale(const GrayScale &gs);
    ~GrayScale() override = default;
    void Apply(BMP &bmp) override;
    std::unique_ptr<Filter> Clone() const override {
        return std::make_unique<GrayScale>(*this);
    }
};

class Negative : public Filter {
public:
    explicit Negative(size_t args_cnt);
    Negative(const Negative &neg);
    ~Negative() override = default;
    void Apply(BMP &bmp) override;
    std::unique_ptr<Filter> Clone() const override {
        return std::make_unique<Negative>(*this);
    }
};

class Sharpening : public Filter {
public:
    explicit Sharpening(size_t args_cnt);
    Sharpening(const Sharpening &sharp);
    ~Sharpening() override = default;
    void Apply(BMP &bmp) override;
    std::unique_ptr<Filter> Clone() const override {
        return std::make_unique<Sharpening>(*this);
    }
};

class EdgeDetection : public Filter {
public:
    explicit EdgeDetection(size_t args_cnt);
    EdgeDetection(const EdgeDetection &edge_detection);
    ~EdgeDetection() override = default;
    void Apply(BMP &bmp) override;
    std::unique_ptr<Filter> Clone() const override {
        return std::make_unique<EdgeDetection>(*this);
    }
};

class GaussianBlur : public Filter {
public:
    explicit GaussianBlur(size_t args_cnt);
    GaussianBlur(const GaussianBlur &gaussian_blur);
    ~GaussianBlur() override = default;
    void Apply(BMP &bmp) override;
    std::unique_ptr<Filter> Clone() const override {
        return std::make_unique<GaussianBlur>(*this);
    }
};

class Anaglyph : public Filter {
public:
    explicit Anaglyph(size_t args_cnt);
    Anaglyph(const Anaglyph &anaglyph);
    ~Anaglyph() override = default;
    void Apply(BMP &bmp) override;
    std::unique_ptr<Filter> Clone() const override {
        return std::make_unique<Anaglyph>(*this);
    }
};

class FilterController {
public:
    template <typename T>
    FilterController &InsertFilter(const T &filter) {
        filter_options_.emplace_back(std::make_unique<T>(filter));
        return *this;
    }

    const std::unique_ptr<Filter> &GiveFilterPattern(const std::string &name);

    std::string GenerateWindow();

private:
    std::vector<std::unique_ptr<Filter>> filter_options_;
};
