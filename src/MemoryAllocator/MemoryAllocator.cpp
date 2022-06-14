//
// Created by Amo on 2022/6/14.
//

#include "Core.h"

void RawMemory::Init(size_t initial_size)
{
    size = initial_size;
    data = (uint8_t *) AmoMemory_Allocate(initial_size);        // allocate a buffer for the data
    offset = 0;     // set the Read Write Cursor's offset to 0
}

void RawMemory::Free()
{
    if (data)
    {
        AmoMemory_Free(data);
        size = 0;
        data = nullptr;
        offset = 0;
    }
}

void RawMemory::ShrinkToFit()
{
    data = (uint8_t *)AmoMemory_ReAlloc( data, offset);
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

void RawMemory::WriteDangerous(const uint8_t *data, size_t data_size)
{
    if (this->offset + data_size >= this->size)     // if the new data size is larger than the current data size,
                                                    // then we reallocate the memory
    {
            size_t new_size = (this->offset + data_size) * 2;

    }
}

void RawMemory::ReadDangerous(uint8_t *data, size_t data_size)
{
    if (this->offset + data_size > this->size)
    {
        AmoLogger_Error("Deserialized bad data. Read boundary out of bounds"
                        ", cannot access '%zu' bytes in memory of size '%zu' bytes",
                        this->offset + data_size,
                        this->size);
        return;
    }

    AmoBase::AmoMemory_CopyMem((uint8_t*)data, this->data + this->offset, data_size);
    this->offset += data_size;
}

template<typename T>
size_t MemoryHelper::CopyDataByType(uint8_t *destin, size_t offset, const T &data)
{
    uint8_t *destin_data = destin + offset;
    *(T*)destin_data = data;
    return sizeof(T);
}



