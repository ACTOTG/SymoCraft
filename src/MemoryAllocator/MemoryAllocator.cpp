//
// Created by Amo on 2022/6/14.
//

#include "Core.h"

void RawMemory::Init(size_t initial_size)
{
    size = initial_size;
    data = (uint8*) g_memory_allocate(initial_size);        // allocate a buffer for the data
    offset = 0;     // set the Read Write Cursor's offset to 0
}

void RawMemory::Free()
{
    if (data)
    {
        g_memory_free(data);
        size = 0;
        data = nullptr;
        offset = 0;
    }
}

void RawMemory::ShrinkToFit()
{
    data = (uint8*)g_memory_realloc( data, offset);
    size = offset;
}

void RawMemory::ResetReadWriteCursor()
{
    offset = 0;
}

void RawMemory::SetCursor(size_t offset)
{
    this->offset = offset;
}

void RawMemory::WriteDangerous(const uint8 *data, size_t data_size)
{
    if (this->offset + data_size >= this->size)     // if the new data size is larger than the current data size,
                                                    // then we reallocate the memory
    {
            size_t new_size = (this->offset + data_size) * 2;

    }
}

