#include <iostream>
#include <Core.h>

int main() {
    AmoBase::AmoMemory_Init( true);
    void *pointer = AmoMemory_Allocate( 50);
    uint8_t* pointer_bytes = (uint8_t*)pointer;
    for (int i = 0; i <= 50; i++)
        pointer_bytes[i] = (uint8_t)'C';
    AmoMemory_Free(pointer);
    return 0;
}
