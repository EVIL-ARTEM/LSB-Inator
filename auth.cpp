#include "auth.h"
#include "fileio.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace auth
{
    // Файл с учётными записями (бинарный)
    static const std::string USERS_FILE = "users.dat";
    // Хэш-функция FNV-1a, 64 бита: h = (h XOR байт) * p mod 2^64
    static std::uint64_t hashPassword(const std::string &password)
    {
        // начальное значение (offset basis) FNV-1a
        std::uint64_t hash = 14695981039346656037ULL;
        for (size_t i = 0; i < password.size(); i++)
        {
            hash = hash ^ static_cast<unsigned char>(password[i]);
            // простое число FNV; переполнение uint64_t и есть mod 2^64
            hash = hash * 1099511628211ULL;
        }
        return hash;
    }
    // Запись о пользователе: логин и хэш строки "логин:пароль"
    struct User
    {
        std::string login;
        std::uint64_t hash;
    };
    // Записывает всех пользователей в users.dat:
    // [1 байт длина логина][логин][8 байт хэша, старший байт первым]
    static void saveUsers(const std::vector<User> &users)
    {
        std::vector<unsigned char> bytes;
        for (const User &u : users)
        {
            bytes.push_back(static_cast<unsigned char>(u.login.size()));
            bytes.insert(bytes.end(), u.login.begin(), u.login.end());
            // раскладываем 64-битный хэш на 8 байт, начиная со старшего
            for (int k = 56; k >= 0; k -= 8)
            {
                bytes.push_back(static_cast<unsigned char>(u.hash >> k) & 0xFF);
            }
        }
        fileio::writeFile(USERS_FILE, bytes);
    }
    // Читает users.dat; если файла нет - пустой список
    static std::vector<User> loadUsers()
    {
        std::vector<User> users;
        std::vector<unsigned char> bytes;
        try
        {
            bytes = fileio::readFile(USERS_FILE);
        }
        catch (const std::exception &)
        {
            return users;
        }
        size_t position = 0;
        while (position < bytes.size())
        {
            size_t lenght = bytes[position];
            position++;
            // запись обрывается раньше, чем должна - файл повреждён
            if (position + lenght + 8 > bytes.size())
            {
                throw std::runtime_error("Файл пользователей повреждён ");
            }
            User u;
            u.login = std::string(bytes.begin() + position, bytes.begin() + position + lenght);
            position += lenght;
            u.hash = 0;
            // собираем хэш обратно из 8 байт
            for (size_t i = 0; i < 8; i++)
            {
                u.hash = (u.hash << 8) | bytes[position];
                position++;
            }
            users.push_back(u);
        }
        return users;
    }
    // Регистрация: проверки логина и пароля, проверка занятости, сохранение
    void registerUser(const std::string &userLogin, const std::string &password)
    {
        if (userLogin.size() == 0)
        {
            throw std::invalid_argument("Логин слишком короткий ");
        }
        if (userLogin.size() > 255)
        {
            throw std::invalid_argument("Логин слишком длинный ");
        }
        // логин - только латинские буквы и цифры
        for (size_t i = 0; i < userLogin.size(); i++)
        {
            if ((userLogin[i] >= '0' && userLogin[i] <= '9') ||
                    (userLogin[i] >= 'a' && userLogin[i] <= 'z') ||
                (userLogin[i] >= 'A' && userLogin[i] <= 'Z'))
            {
                continue;
            }
            else
            {
                throw std::invalid_argument("Логин может содержать в себе только цифры и латинские буквы");
            }
        }
        if (password.size() < 8)
        {
            throw std::invalid_argument("Пароль должен быть не короче 8 символов ");
        }
        // в пароле должны быть и цифры, и буквы (байт > 127 - часть русской буквы)
        bool hasDigit = false;
        bool hasLetter = false;
        for (char c : password)
        {
            unsigned char b = static_cast<unsigned char>(c);
            if (c >= '0' && c <= '9')
            {
                hasDigit = true;
            }
            else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || b > 127)
            {
                hasLetter = true;
            }
        }
        if (!hasDigit || !hasLetter)
        {
            throw std::invalid_argument("Пароль должен содержать цифры и буквы");
        }
        std::vector<User> users = loadUsers();
        for (const User &u : users)
        {
            if (u.login == userLogin)
            {
                throw std::runtime_error("логин уже занят ");
            }
        }
        User user;
        user.login = userLogin;

        // соль - логин: одинаковые пароли разных людей дают разные хэши
        user.hash = hashPassword(userLogin + ":" + password);
        users.push_back(user);
        saveUsers(users);
    }
    // Вход: ищем логин и сравниваем хэши, сам пароль нигде не хранится
    bool login(const std::string &userLogin, const std::string &password)
    {
        std::vector<User> users = loadUsers();
        for (const User &u : users)
        {
            if (u.login == userLogin)
            {
                return hashPassword(userLogin + ":" + password) == u.hash;
            }
        }
        return false;
    }
}