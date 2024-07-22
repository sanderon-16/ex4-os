#include "VirtualMemory.h"
#include "PhysicalMemory.h"


void VMInitialize() {
    // writes 0 on root
    word_t zero = 0;
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(0 + i, zero);
    }
}

int VMread(uint64_t virtualAddress, word_t *value) {

}


int VMwrite(uint64_t virtualAddress, word_t value) {

}
