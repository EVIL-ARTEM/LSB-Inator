#include "fileio.h"
#include <fstream>
#include <stdexcept> // for std::runtime_error

namespace fileio
{
    std::vector<unsigned char> readFile(const std::string &fileName)
    {
        std::ifstream stream(fileName, std::ios::binary);

        if (!stream.is_open())
        {
            throw std::runtime_error("Не удалось открыть файл: " + fileName);
        }

        stream.seekg(0, std::ios::end);
        auto size = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::vector<unsigned char> vec(static_cast<size_t>(size));
        stream.read(reinterpret_cast<char *>(vec.data()), size);
        return vec;
    }

    void writeFile(const std::string &fileName, const std::vector<unsigned char> &data) {
        std::ofstream stream(fileName, std::ios::binary);

        if (!stream.is_open()) {
            throw std::runtime_error("Не удалось создать файл: " + fileName);
        }
        
        stream.write(reinterpret_cast<const char *>(data.data()), data.size());
    }
}