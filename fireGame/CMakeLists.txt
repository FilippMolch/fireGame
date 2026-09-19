cmake_minimum_required(VERSION 3.28.1)
project(fireGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(fireGame
        src/glad.c
        src/main.cpp
        src/HE/HEngine.cpp
        src/HE/shader.cpp
        src/HE/sceneMng.cpp
        src/HE/HECamera.cpp
        include/HE/UIMng.h
        src/HE/UIMng.cpp
        include/HE/HEtools.h
        src/HE/HEtools.cpp
)

target_include_directories(fireGame PUBLIC "include/")

target_link_libraries(fireGame ${CMAKE_SOURCE_DIR}/lib/libbox2d.a)
target_link_libraries(fireGame ${CMAKE_SOURCE_DIR}/lib/libglfw3.a)
target_link_libraries(fireGame ${CMAKE_SOURCE_DIR}/lib/libfreetype.a)
target_link_libraries(fireGame opengl32.lib)
