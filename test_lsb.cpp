#include "lsb.h"
#include "image.h"
#include <iostream>
#include <string>
#include <vector>

int main()
{
    std::string keyText = "MySecretKey123";
    std::vector<unsigned char> stegoKey(keyText.begin(), keyText.end());
    try
    {
        auto firstPNG = image::Picture("original.png");
        std::cout << "Ёмкость 1го: " << lsb::capacity(firstPNG.pixels().size())
                  << " байт" << std::endl;
        std::string text = "Hello, LSB!";
        std::vector<unsigned char> message(text.begin(), text.end());
        lsb::embed(firstPNG.pixels(), message, stegoKey);
        firstPNG.save("stego.png");
        auto secondPNG = image::Picture("stego.png");
        std::cout << "Ёмкость 2го : " << lsb::capacity(secondPNG.pixels().size())
                  << " байт" << std::endl;
        auto extraction = lsb::extract(secondPNG.pixels(), stegoKey);
        if (extraction == message)
        {
            std::cout << "Успех! Совпадение" << std::endl;
            for (const unsigned char i : extraction)
            {
                std::cout << i;
            }
            std::cout << std::endl;
        }
        else
        {
            std::cout << "Не совпадает " << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "ERROR " << e.what() << std::endl;
        return 1;
    }
    try
    {
        auto newPNG = image::Picture("original.png");
        auto extra = lsb::extract(newPNG.pixels(), stegoKey);
        std::cout << "Ошибка: из пустой картинки что-то извлеклось" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Все хорошо, картинка распознана! " << e.what() << std::endl;
    }
    try
    {
        std::string badKeyText = "MyBADKey321!";
        std::vector<unsigned char> badKey(badKeyText.begin(), badKeyText.end());
        auto stegoPNG = image::Picture("stego.png");
        auto stegoExtra = lsb::extract(stegoPNG.pixels(), badKey);
        std::cout << "Ошибка: с чужим ключом что-то извлеклось " << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Чужой ключ отклонён правильно: " << e.what() << std::endl;
    }

    return 0;
}