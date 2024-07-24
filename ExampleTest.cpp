#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#include <cstdio>
#include <cassert>
#include <iostream>

void print_barrier()
{
    std::cout<<"################"<<std::endl;
}

int main(int argc, char **argv) {
    VMinitialize();
    printRam();
    print_barrier();
    VMwrite(13, 3);
    printRam();
    print_barrier();
    word_t value;
    VMread(13, &value);
    std::cout<< "calling VMread(13) returns: " << value<<std::endl;
    printRam();
    print_barrier();
    VMread(6, &value);
    std::cout<< "calling VMread(6) returns: " << value<<std::endl;
    printRam();
    print_barrier();
    VMread(31, &value);
    std::cout<< "calling VMread(31) returns: " << value<<std::endl;
    printRam();
    print_barrier();
    return 0;
}
