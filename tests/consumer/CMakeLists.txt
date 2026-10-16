# A separate project that finds the installed package, as an outside program
# would. The test install.consumer builds it against <build>/stage and runs
# it. A -DCMAKE_MSVC_RUNTIME_LIBRARY on the command line picks the consumer's
# C runtime on MSVC; other compilers ignore it.
cmake_minimum_required(VERSION 3.25)
project(consumer C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(muhtari CONFIG REQUIRED)

add_executable(consumer main.c)
target_link_libraries(consumer PRIVATE muhtari::muhtari)
