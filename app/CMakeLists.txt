cmake_minimum_required(VERSION 3.10)

# Имя проекта
project(BlockCache)

# Стандарт C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка пути для поиска заголовочных файлов
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Создание динамической библиотеки для блочного кэша
add_library(block_cache SHARED BlockCache.cpp api.cpp)

# Исполняемый файл для поиска
add_executable(ema_search_int ema_search_int.cpp)

# Линковка исполняемых файлов с библиотекой блочного кэша
target_link_libraries(ema_search_int block_cache)
