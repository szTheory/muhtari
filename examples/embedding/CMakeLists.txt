cmake_minimum_required(VERSION 3.25)
project(muhtari_embedding_quickstart C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(muhtari CONFIG REQUIRED)

add_executable(quickstart quickstart.c)
target_link_libraries(quickstart PRIVATE muhtari::muhtari)
