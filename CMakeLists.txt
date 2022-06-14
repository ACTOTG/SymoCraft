cmake_minimum_required(VERSION 3.22)
project(SymoCraft)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(MemoryAllocator include/MemoryAllocator/AmoMemoryBase.cpp
                    src/MemoryAllocator/MemoryAllocator.cpp)

add_executable(SymoCraft src/main.cpp
                         ${MemoryAllocator})

add_subdirectory(vendor/glm)
add_subdirectory(vendor/glfw)
target_link_libraries(${PROJECT_NAME} glfw)

include_directories(vendor)
include_directories(include)
include_directories(vendor/glfw/include/)