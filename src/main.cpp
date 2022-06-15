#include <iostream>
#include <Core.h>



int main() {
    AmoBase::AmoMemory_Init( true);
    SizedMemory test;
    int test_flags = 35;
    test = Pack( test_flags);
    int test_result;
    Unpack<int>( test, &test_result);
    std::cout << test_result;
    return 0;
}
