#ifndef AMO_MEMORY_BASE_H
#define AMO_MEMORY_BASE_H
#include <cstdint>
#include <cstddef>


// @Amo: Create an allocator for object's programming with debug mode
// reference to Gabe's cppUtil
//              kiwano's Allocator

namespace AmoMemoryBase{
// ------------------------------------
// Memory Base Functions
// ------------------------------------


// Macro Definition
#define AmoMemory_Allocate(num_bytes)  _AmoMemory_Allocate(__FILE__, __LINE__, num_bytes)
#define AmoMemory_ReAlloc( memory, new_size) _AmoMemory_ReAlloc(__FILE__, __LINE__, memory, new_size)
#define AmoMemory_Free( memory) _AmoMemory_Free(__FILE__, __LINE__, memory)

// Allocate memory function
// Parameters: File name, call allocator line, numbers of bytes
    void *_AmoMemory_Allocate( const char* filename, int line, size_t num_bytes);

// Reallocate memory function
// Parameters: File name, call allocator line, memory pointer, new size of data
    void *_AmoMemory_ReAlloc( const char* filename, int line, void* memory, size_t new_size);

// Free memory function
// Parameters: File name, call allocator line, memory pointer
    void _AmoMemory_Free(const char* filename, int line, void* memory);


// Memory Initializing Function
// Parameters: detect memory leaks,
    void AmoMemory_Init( bool detect_memory_leaks, uint16_t in_buffer_unit = 5);

// Detected memory leak function
    void AmoMemory_MemoryLeaksDetected();

// Memory Comparing function
// Using memcmp
// Parameters: memory1, memory2, numbers of bytes to be compared
    int AmoMemory_CompareMem( void* mem1, void* mem2, size_t num_bytes);

// Memory Zero Setting function
// Set the specified former bits to be zero
// Parameters: memory, number of bytes
    void AmoMemory_ZeroMem( void *memory, size_t num_bytes);

// Memory Copying function
// using memcpy
// Parameters: destination, source, numbers of bytes
    void AmoMemory_CopyMem( void* destin, void* source, size_t num_bytes);

// ------------------------------------
// Logging Functions
// ------------------------------------

// Log level
    enum class LogLevel
    {
        None = 0,
        Log,
        Info,
        Warning,
        Error,
        Notice,
        All
    };


// Macro Definitions
#if !(defined(__GNUC__) || defined(__GNUG__))
    #define AmoLogger_Log(var_format, ...) _AmoLogger_Log(__FILE__, __LINE__, var_format, __VA_ARGS__)
#define AmoLogger_Info(var_format, ...) _AmoLogger_Info(__FILE__, __LINE__, var_format, __VA_ARGS__)
#define AmoLogger_Warning(var_format, ...) _AmoLogger_Warning(__FILE__, __LINE__, var_format, __VA_ARGS__)
#define AmoLogger_Error(var_format, ...) _AmoLogger_Error(__FILE__, __LINE__, var_format, __VA_ARGS__)
#define AmoLogger_Notice(condition, var_format, ...) _AmoLogger_Notice(__FILE__, __LINE__, condition, var_format, __VA_ARGS__)
#else
#define AmoLogger_Log(var_format, ...) _AmoLogger_Log(__FILE__, __LINE__, var_format, ##__VA_ARGS__)
#define AmoLogger_Info(var_format, ...) _AmoLogger_Info(__FILE__, __LINE__, var_format, ##__VA_ARGS__)
#define AmoLogger_Warning(var_format, ...) _AmoLogger_Warning(__FILE__, __LINE__, var_format, ##__VA_ARGS__)
#define AmoLogger_Error(var_format, ...) _AmoLogger_Error(__FILE__, __LINE__, var_format, ##__VA_ARGS__)
#define AmoLogger_Notice(condition, var_format, ...) _AmoLogger_Notice(__FILE__, __LINE__, condition, var_format, ##__VA_ARGS__)
#endif

// Log
// Parameters: file name, code line, variables formats
    void _AmoLogger_Log( const char* filename, int line, const char* var_format, ...);

// Info
// Parameters: file name, code line, variables formats
    void _AmoLogger_Info( const char* filename, int line, const char* var_format, ...);

// Warning
// Parameters: file name, code line, variables formats
    void _AmoLogger_Warning( const char* filename, int line, const char* var_format, ...);

// Error
// Parameters: file name, code line, variables formats
    void _AmoLogger_Error( const char* filename, int line, const char* var_format, ...);

// Notice(Hint)
// Parameters: file name, code line, problem condition, variables formats
    void _AmoLogger_Notice( const char* filename, int line, int condition, const char* var_format, ...);

// LogLevel Setting
// Parameters: Log Level
    void AmoLogger_SetLevel(LogLevel level);

// LogLevel Return
    LogLevel AmoLogger_GetLevel();


}
#endif
