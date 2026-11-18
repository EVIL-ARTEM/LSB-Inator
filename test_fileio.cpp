#include "fileio.h"
#include <iostream>

int main() {
    try {
        auto imagen = fileio::readFile("original.png");
        std::cout << "Прочитано байт: " << imagen.size() << std::endl;
        fileio::writeFile("copy.png", imagen);
        std::cout << "Копия сохранена!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

