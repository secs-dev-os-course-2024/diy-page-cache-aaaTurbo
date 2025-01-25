#ifndef BLOCKCACHE_H
#define BLOCKCACHE_H

#include <cstddef>
#include <unordered_map>
#include <list>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

#define BLOCK_SIZE 4096  // Размер блока кэша

class CacheBlock {
public:
    int fd;             // Файловый дескриптор
    off_t offset;       // Смещение в файле
    char data[BLOCK_SIZE]; // Данные блока
    int frequency;      // Частота доступа

    CacheBlock(int file_descriptor, off_t off, const char *buf);
};

class BlockCache {
public:
    BlockCache(size_t size); // Конструктор для инициализации кэша
    CacheBlock *find(int fd, off_t offset); // Поиск блока в кэше
    void add(int fd, off_t offset, const char *data); // Добавление блока в кэш
    size_t read(int fd, void *buf, size_t count); // Чтение из кэша или файла
    size_t write(int fd, const void *buf, size_t count); // Запись в кэш и файл
    off_t lseek(int fd, off_t offset, int whence); // Перемещение указателя файла
    int fsync(int fd); // Синхронизация данных с диском

private:
    size_t max_cache_size; // Максимальный размер кэша
    std::list<CacheBlock> blocks; // Список блоков кэша
    std::unordered_map<int, std::list<CacheBlock>> cache_map; // Хранение блоков по файловым дескрипторам
};

#endif // BLOCKCACHE_H
