cmake_minimum_required(VERSION 3.22)
project(SymoCraft)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(MemoryAllocator include/MemoryAllocator/AmoBase.cpp)

file(GLOB Core
        "src/Core/*.cpp")

file(GLOB SOURCE_FILE
        "src/renderer/*.cpp"
        "src/Camera/Camera.cpp"
        "vendor/yaml-cpp/src/*.cpp"
        "vendor/yaml-cpp/src/*.h"
        "include/renderer/batch.hpp")

add_executable(SymoCraft src/main.cpp
                         vendor/glad/glad.c
                         ${MemoryAllocator}
                         ${Core}
                         ${SOURCE_FILE})

add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

include_directories(vendor include vendor/glfw/include/)