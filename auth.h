#pragma once

#include <string>

namespace auth
{
    // Регистрирует пользователя: проверяет логин и пароль, сохраняет хэш в users.dat
    // invalid_argument - неверный логин/пароль, runtime_error - логин занят
    void registerUser(const std::string &userLogin, const std::string &password);
    // Возвращает true, если пользователь есть и хэш пароля совпал
    bool login(const std::string &userLogin, const std::string &password);
}