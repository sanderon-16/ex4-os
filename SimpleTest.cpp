#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include <cstdio>
#include <cassert>

int main(int argc, char **argv) {
    VMinitialize();
    long long int run = 2 * NUM_FRAMES;
//    long long int run = 47;
    for (uint64_t i = 0; i < run; ++i) {
        printf("writing to %llu\n", (long long int) i);
        VMwrite(5 * i * PAGE_SIZE, i);
    }

    for (uint64_t i = 0; i < run; ++i) {
        word_t value;
        VMread(5 * i * PAGE_SIZE, &value);
        printf("reading from %llu %d\n", (long long int) i, value);
        assert(uint64_t(value) == i);
    }
    printf("success\n");

    return 0;
}
