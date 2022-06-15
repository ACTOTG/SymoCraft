//
// Created by Amo on 2022/6/14.
//

#ifndef SYMOCRAFT_CORE_H
#define SYMOCRAFT_CORE_H

// Standard
#include <filesystem>
#include <cstring>
#include <iostream>
#include <fstream>
#include <array>
#include <cstdio>
#include <vector>
#include <string>
#include <string_view>
#include <thread>
#include <mutex>
#include <random>
#include <future>
#include <queue>
#include <algorithm>
#include <bitset>
#include <optional>
//#include <unordered_set>
//#include <unordered_map>

// Glm
#define GLM_FORCE_SWIZZLE
#define GLM_EXT_INCLUDED
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/integer.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>


// Use this for hash map and hash sets instead of the crappy std lib
#include <robin_hood.h>

// GLFW/glad
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Amo Memory Allocator
#include <MemoryAllocator/AmoBase.h>
#include <MemoryAllocator/RawMemory.h>
#include <MemoryAllocator/MemoryHelper.h>

// Core
#include <Core/GlobalThreadPool.h>
#include <Core/Window.h>


#include <yaml-cpp/yaml.h>


typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t uint8;

typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;
typedef int8_t int8;

#endif //SYMOCRAFT_CORE_H
