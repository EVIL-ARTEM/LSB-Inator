#pragma once

#include <vector>
#include <cstddef> //чтобы size_t работал на винде

namespace rc4 {
std::vector <unsigned char> crypt(const std::vector <unsigned char> &data, const std::vector <unsigned char> &key);
std::vector <unsigned char> generateKey(size_t length);
}