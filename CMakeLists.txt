cmake_minimum_required(VERSION 3.24)
project(nixvm VERSION 0.0.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Warnings
add_compile_options(-Wall -Wextra)

add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -Oz -D_FORTIFY_SOURCE=3)
add_link_options(-fsanitize=address,undefined -Oz)

add_subdirectory(src)
