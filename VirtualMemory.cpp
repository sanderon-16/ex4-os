#include "VirtualMemory.h"
#include "PhysicalMemory.h"

int get_Paddress(uint64_t address);

void VMInitialize() {
    // writes 0 on root
    word_t zero = 0;
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(0 + i, zero);
    }
}

int VMread(uint64_t virtualAddress, word_t *value) {

}


int VMwrite(uint64_t virtualAddress, word_t value)
{
    int addr = get_Paddress(virtualAddress);
}


int get_Paddress(uint64_t address) {
    int prev_padd = 0;
    return 0;
}
