//
// Created by Amo on 2022/6/14.
//

#ifndef SYMOCRAFT_MEMORYHELPER_H
#define SYMOCRAFT_MEMORYHELPER_H

struct SizedMemory
{
    uint8_t* memory;
    size_t size;
};

namespace MemoryHelper
{
    // Copy data by Type
    // destination's type should equal to data's type
    // Parameters: destination, offset, data
    template<typename T>
    size_t CopyDataByType(uint8_t* destin, size_t offset, const T& data);

    // Copy data to type
    // To deal with the situation that destination type does not fit in data's type
    // Parameters: destination, offset, first data, rest of data
    template<typename First, typename... Rest>
    void CopyDataToType(uint8_t* destin, size_t offset, const First& data, Rest... rest_data);

    // Unpack data
    // unpack the data in sized memory form cursor to first data pointer, ...
    // Parameters: sized memory, cursor offset, first data, ...rest of data
    template<typename First, typename... Rest>
    void UnpackData( SizedMemory& sized_mem, size_t offset, First* data, Rest... rest_data);

}

#endif //SYMOCRAFT_MEMORYHELPER_H
