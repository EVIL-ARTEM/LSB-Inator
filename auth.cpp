#include "auth.h"
#include "fileio.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdint>

namespace auth
{
    static const std::string USERS_FILE = "users.dat";
    static std::uint64_t hashPassword(const std::string &password)
    {
        std::uint64_t hash = 14695981039346656037ULL;
        for (size_t i = 0; i < password.size(); i++)
        {
            hash = hash ^ static_cast<unsigned char>(password[i]);
            hash = hash * 1099511628211ULL;
        }
        return hash;
    }
    struct User
    {
        std::string login;
        std::uint64_t hash;
    };
    static void saveUsers(const std::vector<User> &users)
    {
        std::vector<unsigned char> bytes;
        for (const User &u : users)
        {
            bytes.push_back(static_cast<unsigned char>(u.login.size()));
            bytes.insert(bytes.end(), u.login.begin(), u.login.end());
            for (int k = 56; k >= 0; k -= 8)
            {
                bytes.push_back(static_cast<unsigned char>(u.hash >> k) & 0xFF);
            }
        }
        fileio::writeFile(USERS_FILE, bytes);
    }
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
            if (position + lenght + 8 > bytes.size())
            {
                throw std::runtime_error("Файл пользователей повреждён ");
            }
            User u;
            u.login = std::string(bytes.begin() + position, bytes.begin() + position + lenght);
            position += lenght;
            u.hash = 0;
            for (size_t i = 0; i < 8; i++)
            {
                u.hash = (u.hash << 8) | bytes[position];
                position++;
            }
            users.push_back(u);
        }
        return users;
    }
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

        user.hash = hashPassword(userLogin + ":" + password);
        users.push_back(user);
        saveUsers(users);
    }
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