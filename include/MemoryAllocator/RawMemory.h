//
// Created by Amo on 2022/6/14.
//

#ifndef SYMOCRAFT_RAWMEMORY_H
#define SYMOCRAFT_RAWMEMORY_H
struct RawMemory
{
    uint8* data;        // data location
    size_t size;        // data size
    size_t offset;      // Read Write Cursor's offset

    void Init( size_t initial_size);    // initialize RawMemory
    void Free();                        // free RawMemory
    void ShrinkToFit();                 // re-allocate / shrink the data from 0 - offset
    void ResetReadWriteCursor();        // set the offset to 0
    void SetCursor(size_t offset);      // set Cursor's offset to offset

    void WriteDangerous( const uint8* data, size_t data_size);      // a dangerous writing to memory that may cause a memory leak
                                                                    // if the data_size does not match the data
    void ReadDangerouse( uint8* data, size_t data_size);            // a dangerous reading of memory

    template<typename T>
    void Write(const T* data)           // a safer writing to memory
    {
        WriteDangerous((uint8*)data, sizeof(T));
    }

    template<typename T>
    void Read(T* data)
    {
        ReadDangerouse((uint8*)data, sizeof(T));
    }

};
#endif //SYMOCRAFT_RAWMEMORY_H
