//
// Created by Amo on 2022/6/15.
//

#ifndef SYMOCRAFT_GLOBALTHREADPOOL_H
#define SYMOCRAFT_GLOBALTHREADPOOL_H

enum class Priority : uint8_t
{
    High = 0,
    Medium,
    Low,
    None
};

typedef void (*TaskFunction)(void* data, size_t data_size);
typedef void (*ThreadCallback)(void* data, size_t data_size);

struct ThreadTask
{
    TaskFunction func;  // store a function pointer to task function
    ThreadCallback callback;    // store a function pointer for thread callback
    uint64_t counter;       // queue order of the task
    void* data;         // a pointer to data
    size_t data_size;   // store data size
    Priority priority;  // task's priority
    const char* task_name;  // the name of the task
};

struct CompareThreadTask
{
    // return true means task1's priority is greater than task2
    bool operator() (const ThreadTask& task1, ThreadTask& task2);
};

class GlobalThreadPool
{
public:
    // Constructor of Global Thread Pool
    // Parameters: the Number of threads in global thread pool
    GlobalThreadPool(uint64_t num_threads);

    // Free global

};

#endif //SYMOCRAFT_GLOBALTHREADPOOL_H
