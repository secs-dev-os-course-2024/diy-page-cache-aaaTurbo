#include "api.h"
#include "BlockCache.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

static BlockCache *cache = nullptr;

// Инициализация кэша
void init_cache(size_t cache_size) {
    cache = new BlockCache(cache_size);
}

int lab2_open(const char *path) {
    return open(path, O_RDWR); // Открываю файл для чтения и записи
}

int lab2_close(int fd) {
    return close(fd); // Закрываю файл
}

ssize_t lab2_read(int fd, void *buf, size_t count) {
    if (!cache) {
        std::cerr << "Cache not initialized!" << std::endl;
        return -1;
    }
    return cache->read(fd, buf, count);
}

ssize_t lab2_write(int fd, const void *buf, size_t count) {
    if (!cache) {
        std::cerr << "Cache not initialized!" << std::endl;
        return -1;
    }
    return cache->write(fd, buf, count);
}

off_t lab2_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence); // Перемещение указателя
}

int lab2_fsync(int fd) {
    return cache->fsync(fd); // Синхронизация с диском
}
