#include "menu.h"
#include "fileio.h"
#include "rc4.h"
#include "lsb.h"
#include "image.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace menu
{
    // принимает строку, проверка на пустоту
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

    // от ввода 999999999999999999999999, чтобы программа не упала
    static int readChoice(const std::string &prompt, int min, int max)
    {
        while (true)
        {
            std::string line = readLine(prompt);

            bool onlyDigits = true;
            for (char c : line)
            {
                if (c < '0' || c > '9')
                {
                    onlyDigits = false;
                    break;
                }
            }

            if (onlyDigits && line.size() <= 9)
            {
                int value = std::stoi(line);
                if (value >= min && value <= max)
                {
                    return value;
                }
            }
            std::cout << "Введите число от " << min << " до " << max << "." << std::endl;
        }
    }

    // проверка, чтобы строка заканчивалась ИМЕННО на .png
    static bool endsWith(const std::string &text, const std::string &suffix)
    {
        return text.size() >= suffix.size() &&
               text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    // перевод стороки в вектор
    static std::vector<unsigned char> toBytes(const std::string &text)
    {
        return std::vector<unsigned char>(text.begin(), text.end());
    }

    // перевод вектора в строку
    static std::string toText(const std::vector<unsigned char> &bytes)
    {
        return std::string(bytes.begin(), bytes.end());
    }

    static std::vector<unsigned char> getKey(const std::string &hint, bool genAllowed)
    {
        while (true)
        {
            std::cout << "Ключ " << hint << ". Выберите способ:" << std::endl;
            std::cout << "1 - ввести с клавиатуры" << std::endl;
            std::cout << "2 - прочитать из файла" << std::endl;
            if (genAllowed)
            {
                std::cout << "3 - сгенерировать" << std::endl;
            }

            int maxChoice = 2;
            if (genAllowed)
            {
                maxChoice = 3;
            }
            int choice = readChoice("Ваш выбор: ", 1, maxChoice);

            if (choice == 1)
            {
                std::string line = readLine("Введите ключ " + hint + ": ");
                if (line.size() < 8)
                {
                    std::cout << "Ключ слишком короткий, минимум 8 символов" << std::endl;
                }
                else
                {
                    return toBytes(line);
                }
            }
            else if (choice == 2)
            {
                std::string fileName = readLine("Путь к файлу ключа: ");
                auto file = fileio::readFile(fileName);
                if (file.empty())
                {
                    throw std::runtime_error("Файл ключа пуст");
                }
                return file;
            }
            else
            {
                std::string fileName = readLine("Имя файла для ключа: ");
                std::vector<unsigned char> key = rc4::generateKey(16);
                fileio::writeFile(fileName, key);
                std::cout << "Ключ сохранён в файл " << fileName << std::endl;
                return key;
            }
        }
    }
    static void embedFlow()
    {
        // 1. картинка
        std::string imagePath = readLine("Путь к картинке: ");
        image::Picture picture(imagePath);
        std::cout << "Поместится до " << lsb::capacity(picture.pixels().size()) << " байт" << std::endl;
        // 2. сообщение
        std::cout << "1 - ввести с клавиатуры, 2 - взять из файла" << std::endl;
        int choice = readChoice("Ваш выбор: ", 1, 2);
        std::vector<unsigned char> message;
        if (choice == 1)
        {
            message = toBytes(readLine("Сообщение: "));
        }
        else
        {
            message = fileio::readFile(readLine("Файл с сообщением: "));
        }
        if (message.empty())
        {
            throw std::runtime_error("Сообщение пустое");
        }
        // 3. метка
        std::vector<unsigned char> withMark = toBytes("STG1");
        withMark.insert(withMark.end(), message.begin(), message.end());
        // 4. ключи
        auto rc4Key = getKey("RC4", true);
        auto stegoKey = getKey("Стеганографии", true);
        // 5–6. шифрование и встраивание
        auto encrypted = rc4::crypt(withMark, rc4Key);
        lsb::embed(picture.pixels(), encrypted, stegoKey);
        // 7. имя выходного файла
        std::string outPath;
        while (true)
        {
            outPath = readLine("Имя файла для результата (.png): ");
            if (!endsWith(outPath, ".png"))
            {
                std::cout << "Имя должно заканчиваться на .png" << std::endl;
            }
            else if (outPath == imagePath)
            {
                std::cout << "Нельзя перезаписать исходную картинку" << std::endl;
            }
            else
            {
                break;
            }
        }
        // 8. сохранение
        picture.save(outPath);
        std::cout << "Готово, сообщение спрятано в " << outPath << std::endl;
    }

    static void extractFlow()
    {
        // 1. путь к стегокартинке, загрузить
        std::string stegoImagenPath = readLine("Путь к картинке: ");
        image::Picture stegoImagen(stegoImagenPath);
        // 2. ключ стеганографии и ключ RC4 через getKey
        auto rc4Key = getKey("RC4", false);
        auto stegoKey = getKey("Стеганографии", false);
        // Метка
        std::vector<unsigned char> mark = toBytes("STG1");
        // 3. извлечь через lsb::extract
        auto encrypted = lsb::extract(stegoImagen.pixels(), stegoKey);
        // 4. расшифровать через rc4::crypt
        auto decrypted = rc4::crypt(encrypted, rc4Key);
        if (decrypted.size() < 4)
        {
            throw std::runtime_error("Неверный ключ RC4 ");
        }

        if (decrypted[0] != mark[0] || decrypted[1] != mark[1] ||
            decrypted[2] != mark[2] || decrypted[3] != mark[3])
        {
            throw std::runtime_error("Неверный ключ RC4");
        }
        std::vector<unsigned char> message(decrypted.begin() + 4, decrypted.end());
        int choice = readChoice("Что сделать: вывести на экран - 1 или сохранить в файл - 2? ", 1, 2);
        if (choice == 1)
        {
            std::cout << toText(message) << std::endl;
        }
        else if (choice == 2)
        {
            std::string filek = readLine("Введите путь для сохранения: ");
            fileio::writeFile(filek, message);
            std::cout << "Готово, сообщение сохранено в " << filek << std::endl;
        }
    }

    void start(const std::string &userLogin)
    {
        std::cout << "Здравствуйте, " << userLogin << std::endl;
        while (true)
        {
            std::cout << "Выберите один из пунктов: " << std::endl;
            std::cout << "1 - спрятать сообщение, " << std::endl;
            std::cout << "2 - извлечь сообщение, " << std::endl;
            std::cout << "0 - выход " << std::endl;
            int choice = readChoice("Ваш выбор: ", 0, 2);
            if (choice == 0)
            {
                break;
            }
            try
            {
                if (choice == 1)
                {
                    embedFlow();
                }
                else
                {
                    extractFlow();
                }
            }
            catch (const std::exception &e)
            {
                std::cout << "Ошибка: " << e.what() << std::endl;
            }
        }
    }
}