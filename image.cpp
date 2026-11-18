#include "image.h"
#include "fileio.h"
#include "lodepng.h"
#include <stdexcept> //for error's
#include <string>
#include <vector>

namespace image
{
    Picture::Picture(const std::string &fileName)
    {
        std::vector<unsigned char> fileBytes = fileio::readFile(fileName);
        unsigned error = lodepng::decode(pixels_, width_, height_,
                                         fileBytes, LCT_RGBA, 8);
        if (error)
        {
            throw std::runtime_error("Не удалось разобрать PNG " + fileName + ": " + lodepng_error_text(error));
        }
    }
    void Picture::save(const std::string &fileName) const
    {
        std::vector<unsigned char> pngBytes;
        unsigned error = lodepng::encode(pngBytes, pixels_, width_, height_, LCT_RGBA, 8);
        if (error)
        {
            throw std::runtime_error("Не удалось создать PNG " + fileName + ": " + lodepng_error_text(error));
        }
        fileio::writeFile(fileName, pngBytes);
    }

}