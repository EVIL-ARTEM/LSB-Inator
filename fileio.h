#pragma once

#include <string>
#include <vector>

namespace fileio {

std::vector<unsigned char> readFile(const std::string& fileName);
void writeFile(const std::string& fileName, const std::vector<unsigned char>& data);

}