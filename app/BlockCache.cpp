#include "BlockCache.h"

CacheBlock::CacheBlock(int file_descriptor, off_t off, const char *buf)
        : fd(file_descriptor), offset(off), frequency(1) {
    std::memcpy(data, buf, BLOCK_SIZE);
}

BlockCache::BlockCache(size_t size) : max_cache_size(size) {}

CacheBlock *BlockCache::find(int fd, off_t offset) {
    auto it = cache_map.find(fd);
    if (it != cache_map.end()) {
        for (auto &block : it->second) {
            if (block.offset == offset) {
                block.frequency++;
                return &block; // Возвращаю указатель на блок
            }
        }
    }
    return nullptr;
}

void BlockCache::add(int fd, off_t offset, const char *data) {
    if (blocks.size() >= max_cache_size) {
        // Удаление блока с наименьшей частотой
        auto least_used = std::min_element(blocks.begin(), blocks.end(),
                                           [](const CacheBlock &a, const CacheBlock &b) {
                                               return a.frequency < b.frequency;
                                           });

        // Удаляем блок из cache_map
        auto &block_list = cache_map[least_used->fd];
        block_list.remove_if([&](const CacheBlock &block) { return block.offset == least_used->offset; });

        // Удаляем блок из основного списка
        blocks.erase(least_used);
    }

    // Создаем новый блок и добавляем его в кэш
    blocks.emplace_back(fd, offset, data);
    cache_map[fd].push_back(blocks.back()); // Теперь добавляем ссылку на новый блок
}
size_t BlockCache::read(int fd, void *buf, size_t count) {
    off_t pos = lseek(fd, 0, SEEK_CUR); // Получаем текущую позицию
    CacheBlock *block = find(fd, pos - count); // Поиск блока кэша для позиции, предшествующей текущей
    if (block) {
        std::memcpy(buf, block->data, count); // Копируем данные из кэша
        lseek(fd, pos, SEEK_SET); // Перемещаем указатель обратно к текущей позиции
        return count;
    } else {
        // Читаем из файла
        ssize_t bytes_read = ::read(fd, buf, count);
        if (bytes_read > 0) {
            add(fd, pos - count, static_cast<char *>(buf)); // Добавляем в кэш
        }
        return bytes_read;
    }
}

size_t BlockCache::write(int fd, const void *buf, size_t count) {
    add(fd, lseek(fd, 0, SEEK_CUR), static_cast<const char *>(buf)); // Добавляем в кэш
    return ::write(fd, buf, count); // Записываем в файл
}

off_t BlockCache::lseek(int fd, off_t offset, int whence) {
    return ::lseek(fd, offset, whence); // Перемещение указателя
}

int BlockCache::fsync(int fd) {
    return ::fsync(fd); // Синхронизация с диском
}
