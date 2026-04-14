cmake_minimum_required(VERSION 3.10)

project(xc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(xc
    main.cpp
    lexer.cpp
)

target_include_directories(xc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
