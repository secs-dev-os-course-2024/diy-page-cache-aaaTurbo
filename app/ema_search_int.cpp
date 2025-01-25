#include <iostream>
#include <random>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <sys/fcntl.h>
#include "api.h" // Подключаю API

size_t FileSize = 256 * (1ULL << 20ULL) / sizeof(int64_t);  // 256 Мб
const int64_t ToFind = 777;

void prepare(size_t fileSize) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dis(1, fileSize - 1);

    std::vector<int64_t> output(fileSize);

    for (size_t i = 0; i < fileSize; ++i) {
        output[i] = dis(gen);
    }
    const size_t place = dis(gen);
    output[place] = ToFind;  // Найти это значение в случайном месте

    int fd = lab2_open("input.bin"); // Открываю файл для записи
    if (fd < 0) {
        throw std::runtime_error("Не удалось открыть выходной файл");
    }

    lab2_write(fd, output.data(), sizeof(int64_t) * fileSize); // Записываю данные
    lab2_close(fd); // Закрываю файл

    std::cout << "Размер файла: " << fileSize * sizeof(int64_t) << " байт\n";
    std::cout << "Значение помещено в позицию: " << place << '\n';
}

size_t compute(size_t fileSize) {
    int fd = lab2_open("input.bin"); // Открываю файл только для чтения
    if (fd < 0) {
        throw std::runtime_error("Не удалось открыть входной файл");
    }

    if (fcntl(fd, F_NOCACHE, 1) == -1) {
        perror("Failed to set F_NOCACHE");
        lab2_close(fd);
        return 1;
    }

    size_t found = 0;
    size_t block = 0;
    std::vector<int64_t> buffer(fileSize); // Размер буфера для чтения

    while (found == 0) {
        ssize_t num_read = lab2_read(fd, buffer.data(), sizeof(int64_t) * buffer.size());
        if (num_read < 0) {
            lab2_close(fd);
            throw std::runtime_error("Ошибка чтения из файла");
        }
        if (num_read == 0) {
            break; // Конец файла
        }

        for (size_t i = 0; i < num_read / sizeof(int64_t); ++i) {
            if (buffer[i] == ToFind) {
                found = (buffer.size() * block) + i;
                break;
            }
        }
        block++;
    }
    lab2_close(fd);
    std::cout << "Найдено на позиции: " << found << '\n';
    return found;
}

int main(int argc, char *argv[]) {
    size_t fileSize = 256 * (1ULL << 20ULL) / sizeof(int64_t);  // 256 Мб

    if (argc == 2) {
        fileSize = std::stoul(argv[1]) * (1ULL << 20ULL) / sizeof(int64_t); // Установить размер файла, если указан
    }

    // Инициализация кэша
    init_cache(10);

    prepare(fileSize); // Подготовка файла

    size_t found_index = compute(fileSize); // Поиск значения в файле

    std::cout << "Значение " << ToFind << " найдено на позиции: " << found_index << std::endl;

    return 0;
}
