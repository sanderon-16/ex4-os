#include "VirtualMemory.h"
#include "PhysicalMemory.h"

typedef uint64_t paddress;

paddress get_Paddress(uint64_t address);
paddress grab_frame(paddress physical_address);
int get_priority_of_frame(paddress frame);


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

paddress grab_frame(paddress skip_this_frame){
    int best_priority = 4;
    paddress chosen_frame = 0;
    for (int frame = 0; frame < NUM_FRAMES; ++frame) {
        if (frame == skip_this_frame)
        {
            continue;
        }
        // calculate priority:
        int priority = get_priority_of_frame(frame);
        if (priority < best_priority){
            best_priority = priority;
            chosen_frame = frame;
        }
    }
    // TODO this:
    switch (best_priority) {
        case 1:
            // remove the reference to this frame from its parent.
            break;
        case 2:
            // an unused frame
            break;
        case 3:
            // all frames are already in use,
            break;
        default:
            // exception
            break;
    }
    return 0
}

int get_priority_of_frame(paddress frame)
{
    // TODO this:
    return 1;
}
