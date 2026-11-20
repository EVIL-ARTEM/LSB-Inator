#pragma once

#include <string>
#include <vector>

 namespace image {
    // Изображение PNG в памяти: размеры и пиксели RGBA (4 байта на пиксель)
    class Picture {
    public:
        // Загружает PNG из файла; runtime_error, если файл не PNG
        explicit Picture(const std::string& fileName);

        // Кодирует пиксели обратно в PNG и сохраняет в файл
        void save(const std::string& fileName) const;

        unsigned width() const { return width_; }
        unsigned height() const { return height_; }
        // Доступ к пикселям: изменяемый (для встраивания) и только для чтения
        std::vector<unsigned char>& pixels() { return pixels_; }
        const std::vector<unsigned char>& pixels() const { return pixels_; }

    private:
        unsigned width_ = 0;
        unsigned height_ = 0;
        std::vector<unsigned char> pixels_;
    };

 }