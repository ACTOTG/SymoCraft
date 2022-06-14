//
// Created by Amo on 2022/6/14.
//

#include "AmoMemoryBase.h"
#include <memory>
#include <vector>
#include <mutex>
#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <chrono>
#include <thread>
#include <array>
#include <algorithm>
#include <cstring>

#include <windows.h>
#include <crtdbg.h>


// ------------------------------------
// Memory Base Implementation
// ------------------------------------
struct DebugMemoryAllocation
{
    const char* file_allocator;     // the file of the allocator
    int file_allocator_line;        // the line of the allocator
    int references;                 // references: 0
    size_t memory_size;
    void* memory;

    bool operator ==(const DebugMemoryAllocation& other) const
    {
        return other.memory == this->memory;
    }
};

static std::mutex memory_mtx;   // memory mutex
static std::vector<DebugMemoryAllocation> allocations;  // allocations vectors
static bool track_memory_allocations = false;       // memory tracking mode
static const std::array<uint8_t, 8> special_memory_flags = { (uint8_t)'A', (uint8_t)'M', (uint8_t)'O', (uint8_t)'F', (uint8_t)'L', (uint8_t)'A', (uint8_t)'G', (uint8_t)'S' };
static uint16_t buffer_unit = 5;

void AmoMemoryBase::AmoMemory_Init(bool detect_memory_leaks, uint16_t in_buffer_unit)
{
    track_memory_allocations = detect_memory_leaks;
    buffer_unit = in_buffer_unit;
}

void* AmoMemoryBase::_AmoMemory_Allocate(const char *filename, int line, size_t num_bytes)
{
    if (track_memory_allocations)
    {
        // Memory tracking mode
        // add sentinel values to the beginning and end of the block of memory
        // to ensure it doesn't have any errors on free
        num_bytes += (buffer_unit * 2) * sizeof(uint8_t);

        // allocate 10 extra bytes, 5 before the block and 5 after.
        // use these to detect Buffer overruns or under-runs
        void* memory = std::malloc(num_bytes);
        if (memory)
        {
            // cursor of memory
            uint8_t* memory_bytes = (uint8_t*)memory;
            for (int i = 0; i < buffer_unit; i++)
                memory_bytes[i] = special_memory_flags[i % special_memory_flags.size()];    // add sentinel values to the front

            memory_bytes = ((uint8_t*)memory) + num_bytes - buffer_unit;
            for (int i = 0; i < buffer_unit; i++)
                memory_bytes[i] = special_memory_flags[i % special_memory_flags.size()];    // add sentinel values to the back
        }

        std::lock_guard<std::mutex> lock(memory_mtx);
        // in debug build, track all memory allocations to see
        // if we free them all as well

        // find the exact allocation in allocation-vector
        auto iterator = std::find(allocations.begin(), allocations.end(), DebugMemoryAllocation{ filename, line, 0, num_bytes, memory});


        if (iterator == allocations.end())
        {
            // if not found, create a new allocation at the end of allocation-vector
            allocations.emplace_back( DebugMemoryAllocation{ filename, line, 1, num_bytes, memory });
        }
        else
        {
            if (iterator->references <= 0)
            {
                iterator->references++;
                iterator->file_allocator = filename;
                iterator->memory_size = num_bytes;
            }
        }
    }
}