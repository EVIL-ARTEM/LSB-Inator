#pragma once

#include <vector>
#include <cstddef> //чтобы size_t работал на винде

namespace rc4 {
// Шифрует или расшифровывает RC4 (одна и та же операция: XOR с гаммой)
std::vector <unsigned char> crypt(const std::vector <unsigned char> &data, const std::vector <unsigned char> &key);
// Генерирует случайный ключ заданной длины
std::vector <unsigned char> generateKey(size_t length);
}