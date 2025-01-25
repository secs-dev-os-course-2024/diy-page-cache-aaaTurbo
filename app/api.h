#ifndef API_H
#define API_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_cache(size_t cache_size);

// Открытие файла
int lab2_open(const char *path);

// Закрытие файла
int lab2_close(int fd);

// Чтение данных из файла
ssize_t lab2_read(int fd, void *buf, size_t count);

// Запись данных в файл
ssize_t lab2_write(int fd, const void *buf, size_t count);

// Перемещение указателя на данные файла
off_t lab2_lseek(int fd, off_t offset, int whence);

// Синхронизация данных из кэша с диском
int lab2_fsync(int fd);

#ifdef __cplusplus
}
#endif

#endif // API_H

