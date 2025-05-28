cmake_minimum_required(VERSION 3.31.6)
project(dapper_dasher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Search raylib
find_library(RAYLIB raylib HINTS /opt/homebrew/lib /usr/local/lib)
if (NOT RAYLIB)
    message(FATAL_ERROR "Raylib does not exists")
endif ()

# Include raylib folders
include_directories(/opt/homebrew/include /usr/local/include)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${RAYLIB} "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")

file(COPY ${CMAKE_SOURCE_DIR}/textures DESTINATION ${CMAKE_BINARY_DIR})