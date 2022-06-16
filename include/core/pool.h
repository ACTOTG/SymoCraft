//
// Created by Amo on 2022/6/15.
//

#ifndef SYMOCRAFT_POOL_H
#define SYMOCRAFT_POOL_H
#include "core.h"

namespace SymoCraft
{

    // Pool
    // a Class Template
    // for Memory Allocator in multiple thread mode

    template<typename T>
    class Pool
    {
    public:

        // Default constructor
        Pool();

        // Constructor
        // Parameters: Pool size, number of pools
        Pool( uint32 _pool_size, uint32 _num_pools);

        // Destructor
        ~Pool();

        // []operator overloading
        // Parameters: [Pool Index]
        T* operator[](int pool_index);
        const T* operator[](int pool_index) const;

        // Get New Pool pointer
        T* GetNewPool();

        // Free Pool
        // Parameters: Pool Index
        void FreePool(uint32 pool_index);

        // Free Pool Overloading
        // Parameters: Pool Pointer
        void FreePool(T* pool);

        // Count, return free_list_size
        uint32 Count();

        // Size, Return num_pools
        uint32 GetNumPool() const;

        //

    private:
        std::mutex free_list_mtx;   // Free list mutex
        uint32 free_list_start;     // Beginning of free list
        uint32 free_list_size;      // Size of free list
        T* * free_list;             // Free list pointer. A pointer to T*

        uint64 data_length;
        uint32 pool_size;
        uint32 num_pools;
        T* data;                    // Pointer to data

    };

}

#endif //SYMOCRAFT_POOL_H
