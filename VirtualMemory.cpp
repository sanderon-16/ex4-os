#include "VirtualMemory.h"
#include "PhysicalMemory.h"

int get_Paddress(uint64_t address);

int get_by_priority(int padd);

void VMInitialize() {
    // writes 0 on root
    word_t zero = 0;
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(0 + i, zero);
    }
}

int VMread(uint64_t virtualAddress, word_t *value) {

    if (virtualAddress >> VIRTUAL_ADDRESS_WIDTH != 0) {
        return 0;
    }

    word_t addr = get_Paddress(virtualAddress);
    uint64_t shift = UINT64_WIDTH - OFFSET_WIDTH;
    PMread(addr * PAGE_SIZE + ((virtualAddress << shift) >> shift), value);
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {

    if (virtualAddress >> VIRTUAL_ADDRESS_WIDTH != 0) {
        return 0;
    }

    word_t addr = get_Paddress(virtualAddress);
    uint64_t shift = UINT64_WIDTH - OFFSET_WIDTH;
    PMwrite(addr * PAGE_SIZE + ((virtualAddress << shift) >> shift), value);
    return 1;
}


word_t get_Paddress(uint64_t Vaddress) {

    int last_padd = 0;
    int cur_padd = 0;

    for (int offsets = 0; offsets < TABLES_DEPTH; offsets++) {

        uint64_t sh_left = (UINT64_WIDTH - (TABLES_DEPTH + 1 + offsets) * OFFSET_WIDTH);
        uint64_t sh_right = (UINT64_WIDTH - OFFSET_WIDTH);
        uint64_t rel_vadd = (Vaddress << sh_left) >> sh_right;

        PMread(cur_padd * PAGE_SIZE + rel_vadd, &cur_padd);

        if (cur_padd == 0) {

            word_t frame = get_by_priority(cur_padd);

            if (offsets + 1 == TABLES_DEPTH) {
                PMrestore(frame, Vaddress);
            } else {
                for (int i = 0; i < PAGE_SIZE; i++) {
                    PMwrite(frame + i, 0);
                }
            }

            PMwrite(last_padd * PAGE_SIZE + rel_vadd, frame);
            cur_padd = frame;
        }
        last_padd = cur_padd;
    }

    return cur_padd;
}

word_t get_by_priority(int padd) {
    return 0;
}
