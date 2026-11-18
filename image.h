#pragma once

#include <string>
#include <vector>

 namespace image {
    class Picture {
    public:
        explicit Picture(const std::string& fileName);

        void save(const std::string& fileName) const;

        unsigned width() const { return width_; }
        unsigned height() const { return height_; }
        std::vector<unsigned char>& pixels() { return pixels_; }
        const std::vector<unsigned char>& pixels() const { return pixels_; }

    private:
        unsigned width_ = 0;
        unsigned height_ = 0;
        std::vector<unsigned char> pixels_;
    };

 }