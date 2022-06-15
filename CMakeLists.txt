cmake_minimum_required(VERSION 3.22)
project(SymoCraft)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(MemoryAllocator include/MemoryAllocator/AmoBase.cpp)

set(Core src/Core/global_thread_pool.cpp
        src/Core/window.cpp
        src/Core/application.cpp)

file(GLOB SOURCE_FILE
        "src/renderer/*.cpp"
        "src/Camera/Camera.cpp"
        "vendor/yaml-cpp/src/*.cpp"
        "vendor/yaml-cpp/src/*.h"
        "include/renderer/batch.hpp"
        "src/Input/*.cpp")

add_executable(SymoCraft src/main.cpp
                         vendor/glad/glad.c
                         ${SOURCE_FILE}
                         ${MemoryAllocator}
                         ${Core})

add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

include_directories(vendor include vendor/glfw/include/)