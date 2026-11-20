#ifdef _WIN32
#include <windows.h>
#endif

#include "menu.h"
#include "auth.h"
#include <string>
#include <stdexcept>
#include <iostream>

// Читает непустую строку с клавиатуры; при закрытом вводе (Ctrl+D) бросает runtime_error
static std::string readLine(const std::string &prompt)
{
    while (true)
    {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line))
        {
            throw std::runtime_error("Ввод прерван");
        }
        if (!line.empty())
        {
            return line;
        }
        std::cout << "Пустой ввод, попробуйте снова." << std::endl;
    }
}
// Заставка: название программы, назначение и автор
void printSplash()
{
    std::cout << "==================================================\n"
              << "                   LSB-Inator\n"
              << "  Скрытая передача сообщений в изображениях PNG\n"
              << "          (LSB-стеганография + шифр RC4)\n"
              << "\n"
              << "    Автор: Мерзликин А. Е., группа АБс-524\n"
              << "            НГТУ, кафедра ЗИ, 2026\n"
              << "==================================================\n\n";
}

// Точка входа: заставка, меню входа и регистрации, не более 3 попыток входа
int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    printSplash();
    try
    {
        // Внешний try ловит любые исключения (прерванный ввод, повреждённый users.dat)
        size_t attempts = 0;
        while (attempts < 3)
        {
            std::string userLogin;
            std::string password;
            std::cout << "1 - вход, 2 - регистрация, 0 - выход " << std::endl;
            std::string choice = readLine("Ваш выбор: ");
            if (choice == "1")
            {
                userLogin = readLine("Ваш логин: ");
                password = readLine("Ваш пароль: ");
                // Сравниваем хэш введённого пароля с сохранённым
                auto autentification = auth::login(userLogin, password);
                if (autentification)
                {
                    menu::start(userLogin);
                    return 0;
                }
                else
                {
                    attempts++;
                    std::cout << "Введен неверный логин или пароль, попыток осталось: " << 3 - attempts << std::endl;
                }
            }
            else if (choice == "2")
            {
                std::string loginUser = readLine("Придумайте имя пользователя: ");
                while (true)
                {
                    // Пароль спрашиваем, пока он не пройдёт проверку
                    std::string parol = readLine("Придумайте пароль: ");
                    try
                    {
                        auth::registerUser(loginUser, parol);
                        std::cout << "Пользователь " << loginUser << " зарегистрирован " << std::endl;
                        break;
                    }
                    // Неверный логин или пароль - просим ввести пароль заново
                    catch (const std::invalid_argument &e)
                    {
                        std::cout << "Ошибка: " << e.what() << std::endl;
                    }
                    // Логин уже занят - возвращаемся в меню входа
                    catch (const std::runtime_error &e)
                    {
                        std::cout << "Ошибка: " << e.what() << std::endl;
                        break;
                    }
                }
            }
            else if (choice == "0")
            {
                return 0;
            }
            else
            {
                std::cout << "Введите 0, 1 или 2" << std::endl;
            }
        }
        if (attempts == 3)
        {
            std::cout << "Превышено число попыток (3) " << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << std::endl
                  << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    return 0; // for main
}