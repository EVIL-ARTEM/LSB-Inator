#include "auth.h"
#include <iostream>
#include <string>
#include <stdexcept>

//rm -f users.dat

int main()
{
    std::string nameFirst = "Artem123";
    std::string passwordFirst = "1qwemnb2!lm";
    std::string nameSec = "Lester123";
    std::string passwordSec = "1q124=sdgsvs";

    try
    {
        auth::registerUser(nameFirst, passwordFirst);
        auth::registerUser(nameSec, passwordSec);
        std::cout << "0. Регистрация двух пользователей: ОК" << std::endl;

        if (auth::login(nameFirst, passwordFirst))
            std::cout << "1. Верный пароль первого: ОК" << std::endl;
        else
            std::cout << "1. Верный пароль первого: ОШИБКА" << std::endl;

        if (auth::login(nameSec, passwordSec))
            std::cout << "2. Верный пароль второго: ОК" << std::endl;
        else
            std::cout << "2. Верный пароль второго: ОШИБКА" << std::endl;

        if (!auth::login(nameFirst, "wesaf1"))
            std::cout << "3. Неверный пароль: ОК" << std::endl;
        else
            std::cout << "3. Неверный пароль: ОШИБКА" << std::endl;

        if (!auth::login("asdgfs", passwordFirst))
            std::cout << "4. Неизвестный логин: ОК" << std::endl;
        else
            std::cout << "4. Неизвестный логин: ОШИБКА" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    try
    {
        auth::registerUser(nameFirst, passwordFirst);
        std::cout << "5. Повторная регистрация: ОШИБКА" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "5. Повторная регистрация: ОК (" << e.what() << ")" << std::endl;
    }

    try
    {
        std::string noneName;
        std::string nonePassword = "ewae5";
        auth::registerUser(noneName, nonePassword);
        std::cout << "6. Пустой логин: ОШИБКА" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "6. Пустой логин: ОК (" << e.what() << ")" << std::endl;
    }

    return 0;
}