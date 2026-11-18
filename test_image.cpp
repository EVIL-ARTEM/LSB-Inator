#include "image.h"
#include <string>
#include <vector>
#include <iostream>

int main()
{
    try
    {
        auto firstPNG = image::Picture("original.png");
        std::cout << "Высота: " << firstPNG.height() << std::endl;
        std::cout << "Ширина: " << firstPNG.width() << std::endl;
        std::cout << "Количество байтов: " << firstPNG.pixels().size() << std::endl;
        if (firstPNG.pixels().size() == firstPNG.height() * firstPNG.width() * 4)
        {
            std::cout << "Размер верный " << std::endl;
        }
        else
        {
            std::cout << "Размер не верный " << std::endl;
        }
        firstPNG.save("copy_image.png");

        auto secondPNG = image::Picture("copy_image.png");
        std::cout << "Высота: " << secondPNG.height() << std::endl;
        std::cout << "Ширина: " << secondPNG.width() << std::endl;
        std::cout << "Количество байтов: " << secondPNG.pixels().size() << std::endl;
        if (secondPNG.pixels().size() == secondPNG.height() * secondPNG.width() * 4)
        {
            std::cout << "Размер верный " << std::endl;
        }
        else
        {
            std::cout << "Размер не верный " << std::endl;
        }
        if (secondPNG.pixels() == firstPNG.pixels())
        {
            std::cout << "ALL RIGT!" << std::endl;
        }
        else
        {
            std::cout << "YOU'VE PROBLEMS" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}