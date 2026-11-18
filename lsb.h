#pragma once

#include <vector>
#include <cstddef> //size_t


namespace lsb
{   
    size_t capacity(size_t bytes);
    void embed(std::vector<unsigned char> &pixels, const std::vector<unsigned char> &message,  const std::vector<unsigned char> &key);
    std::vector<unsigned char> extract(const std::vector<unsigned char> &pixels, const std::vector<unsigned char> &key);
}