#include "lsb.h"
#include <vector>
#include <random>
#include <utility>
#include <cstddef>   //size_t
#include <stdexcept> //for error

namespace lsb
{
    // Перемешивает номера всех бит по ключу (Фишер - Йейтс "изнутри наружу").
    // Одинаковый ключ - одинаковый порядок. % вместо distribution,
    // чтобы порядок совпадал на любом компиляторе
    static std::vector<size_t> mixer(size_t positionBits, const std::vector<unsigned char> &key)
    {
        if (key.size() == 0)
        {
            throw std::invalid_argument("Ключ пуст ");
        }
        std::vector<size_t> numbers(positionBits);
        // генератор mt19937, начальное состояние из байт ключа
        std::seed_seq seed(key.begin(), key.end());
        std::mt19937 gen(seed);
        for (size_t i = 0; i < numbers.size(); i++)
        {
            numbers[i] = i;
            size_t j = gen() % (i + 1);
            std::swap(numbers[i], numbers[j]);
        }
        return numbers;
    }
    // Номер бита -> номер байта в RGBA: пиксель = n/3, канал = n%3 (альфу пропускаем)
    static size_t byteIndex(size_t bitNumber)
    {
        return (bitNumber / 3) * 4 + bitNumber % 3;
    }
    // Записывает бит в младший бит канала: (c AND 11111110) OR bit
    static void writeBit(std::vector<unsigned char> &pixels, size_t bitNumber, unsigned char bit)
    {
        size_t i = byteIndex(bitNumber);
        pixels[i] = ((pixels[i] & 0xFE) | bit);
    }
    // Читает младший бит канала
    static unsigned char readBit(const std::vector<unsigned char> &pixels, size_t bitNumber)
    {
        return pixels[byteIndex(bitNumber)] & 1;
    }
    // Ёмкость в байтах: пиксели * 3 бита / 8 - 6 байт заголовка
    size_t capacity(size_t bytes)
    {
        size_t pixels = bytes / 4;
        size_t bits = pixels * 3;
        size_t fitBytes = bits / 8;
        if (fitBytes < 6)
        {
            return 0;
        }
        else
        {
            return fitBytes - 6;
        }
    }
    // Встраивание: заголовок (4 байта длины + 2 байта суммы) + сообщение
    void embed(std::vector<unsigned char> &pixels, const std::vector<unsigned char> &message, const std::vector<unsigned char> &key)
    {
        if (message.size() > capacity(pixels.size()))
        {
            throw std::runtime_error("Сообщение не помещается в изображение");
        }
        size_t totalBits = (pixels.size() / 4) * 3;
        std::vector<size_t> positions = mixer(totalBits, key);
        // контрольная сумма - сумма байт по модулю 65536
        size_t summa = 0;
        for (size_t i = 0; i < message.size(); i++)
        {
            summa = (summa + message[i]) % 65536;
        }
        // длина и сумма записываются старшим байтом вперёд
        std::vector<unsigned char> data;
        data.push_back((message.size() >> 24) & 0xFF);
        data.push_back((message.size() >> 16) & 0xFF);
        data.push_back((message.size() >> 8) & 0xFF);
        data.push_back(message.size() & 0xFF);
        data.push_back((summa >> 8) & 0xFF);
        data.push_back(summa & 0xFF);
        data.insert(data.end(), message.begin(), message.end());

        // каждый бит данных пишем в свою перемешанную позицию, от старшего бита к младшему
        size_t bitNumber = 0;
        for (unsigned char i : data)
        {
            for (int k = 7; k >= 0; k--)
            {
                unsigned char bit = (i >> k) & 1;
                writeBit(pixels, positions[bitNumber], bit);
                bitNumber++;
            }
        }
    }
    // Собирает байт из 8 следующих бит (bitNumber сдвигается)
    static unsigned char readByte(const std::vector<unsigned char> &pixels, size_t &bitNumber, const std::vector<size_t> &positions)
    {
        unsigned char byte = 0;
        for (size_t i = 0; i < 8; i++)
        {
            byte = (byte << 1) | readBit(pixels, positions[bitNumber]);
            bitNumber++;
        }
        return byte;
    }
    // Извлечение: те же позиции по ключу, читаем заголовок, потом сообщение
    std::vector<unsigned char> extract(const std::vector<unsigned char> &pixels, const std::vector<unsigned char> &key)
    {
        if ((pixels.size() / 4) * 3 < 48)
        {
            throw std::runtime_error("Картинка слишком мала, чтобы содержать сообщение ");
        }
        size_t totalBits = (pixels.size() / 4) * 3;
        std::vector<size_t> positions = mixer(totalBits, key);
        size_t bitNumber = 0;
        unsigned char b0 = readByte(pixels, bitNumber, positions);
        unsigned char b1 = readByte(pixels, bitNumber, positions);
        unsigned char b2 = readByte(pixels, bitNumber, positions);
        unsigned char b3 = readByte(pixels, bitNumber, positions);
        size_t length = (static_cast<size_t>(b0) << 24) | (static_cast<size_t>(b1) << 16) | (static_cast<size_t>(b2) << 8) | static_cast<size_t>(b3);
        unsigned char s0 = readByte(pixels, bitNumber, positions);
        unsigned char s1 = readByte(pixels, bitNumber, positions);
        size_t saveSumma = (static_cast<size_t>(s0) << 8) | static_cast<size_t>(s1);
        // неверный ключ даёт случайную длину - обычно больше ёмкости
        if (length > capacity(pixels.size()))
        {
            throw std::runtime_error("В картинке нет сообщения или данные повреждены ");
        }
        std::vector<unsigned char> message(length);
        for (size_t i = 0; i < length; i++)
        {
            message[i] = readByte(pixels, bitNumber, positions);
        }
        size_t summa = 0;
        for (size_t i = 0; i < message.size(); i++)
        {
            summa = (summa + message[i]) % 65536;
        }
        // данные изменились после встраивания
        if (saveSumma != summa)
        {
            throw std::runtime_error("Контрольная сумма не совпадает ");
        }
        return message;
    }
}