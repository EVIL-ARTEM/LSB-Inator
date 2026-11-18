#include "rc4.h"
#include "fileio.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

int main()
{
    try
    {
        std::string s = "Key";
        std::vector<unsigned char> key(s.begin(), s.end());
        std::string t = "Plaintext";
        std::vector<unsigned char> text(t.begin(), t.end());
        auto cripta = rc4::crypt(text, key);
        for (unsigned char b : cripta)
        {
            std::cout << std::hex << std::uppercase
                      << std::setw(2) << std::setfill('0')
                      << static_cast<int>(b) << ' ';
        }
        std::cout << std::dec << '\n';
        auto ANTIcripta = rc4::crypt(cripta, key);
        for (unsigned char z : ANTIcripta)
        {
            std::cout << std::hex << std::uppercase
                      << std::setw(2) << std::setfill('0')
                      << static_cast<int>(z) << ' ';
        }
        std::cout << std::dec << '\n';
        if (text == ANTIcripta)
        {
            std::cout << "Просто лучший, всё круто!" << '\n';
        }
        else
        {
            std::cout << "Ошибочка! " << '\n';
        }
        auto imagen = fileio::readFile("original.png");
        auto randomKey = rc4::generateKey(16);
        auto shifr = rc4::crypt(imagen, randomKey);
        auto DEshifr = rc4::crypt(shifr, randomKey);
        if (DEshifr == imagen)
        {
            std::cout << "Ключ работает! " << '\n';
        }
        else
        {
            std::cout << "Ошибка! " << '\n';
        }
        std::cout << randomKey.size() << '\n';
    }
    catch (const std::exception &e)
    {
        std::cout << "ERROR! " << e.what() << std::endl;
        return 1;
    }
    return 0;
}