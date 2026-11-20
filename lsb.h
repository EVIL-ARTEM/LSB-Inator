#pragma once

#include <vector>
#include <cstddef> //size_t


namespace lsb
{   
    // Сколько байт сообщения поместится: 3 бита на пиксель минус 6 байт заголовка
    size_t capacity(size_t bytes);
    // Прячет сообщение в младшие биты R, G, B в позициях, перемешанных ключом
    void embed(std::vector<unsigned char> &pixels, const std::vector<unsigned char> &message,  const std::vector<unsigned char> &key);
    // Достаёт сообщение тем же ключом; runtime_error, если ключ неверный или данные повреждены
    std::vector<unsigned char> extract(const std::vector<unsigned char> &pixels, const std::vector<unsigned char> &key);
}