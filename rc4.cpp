#include "rc4.h"
#include <vector>
#include <array>
#include <stdexcept>
#include <random>
#include <utility> //for swap

namespace rc4
{

    static std::array<unsigned char, 256> initState(const std::vector<unsigned char> &key)
    {
        if (key.empty())
        {
            throw std::invalid_argument("Ключ RC4 не может быть пустым ");
        }
        std::array<unsigned char, 256> S;
        for (int i = 0; i < 256; i++)
        {
            S[i] = i;
        }
        int j = 0;
        for (int i = 0; i < 256; i++)
        {
            j = (j + S[i] + key[i % key.size()]) % 256;
            std::swap(S[i], S[j]);
        }
        return S;
    }

    std::vector<unsigned char> crypt(const std::vector<unsigned char> &data, const std::vector<unsigned char> &key)
    {
        std::array<unsigned char, 256> S = initState(key);
        std::vector<unsigned char> result(data.size());

        int i = 0;
        int j = 0;
        for (size_t k = 0; k < data.size(); k++)
        {
            i = (i + 1) % 256;
            j = (j + S[i]) % 256;
            std::swap(S[i], S[j]);
            int gammaByte = S[(S[i] + S[j]) % 256];
            result[k] = data[k] ^ gammaByte;
        }
        return result;
    }

    std::vector<unsigned char> generateKey(size_t length)
    {
        if (length == 0)
        {
            throw std::invalid_argument("Длина ключа должна быть больше 0 ");
        }
        std::vector<unsigned char> key(length);
        std::random_device source;
        std::uniform_int_distribution<int> byteDist(0, 255);
        for (size_t i = 0; i < key.size(); i++)
        {
            key[i] = static_cast<unsigned char>(byteDist(source));
        }
        return key;
    }
}