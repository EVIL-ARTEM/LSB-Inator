#pragma once

#include <string>

namespace auth
{
    void registerUser(const std::string &userLogin, const std::string &password);
    bool login(const std::string &userLogin, const std::string &password);
}