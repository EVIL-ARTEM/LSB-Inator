#pragma once

#include <string>
#include <vector>

namespace fileio {

// Читает файл целиком в бинарном режиме; runtime_error, если не открылся
std::vector<unsigned char> readFile(const std::string& fileName);
// Записывает байты в файл в бинарном режиме; runtime_error, если не создался
void writeFile(const std::string& fileName, const std::vector<unsigned char>& data);

}