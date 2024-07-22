#include "VirtualMemory.h"
#include "PhysicalMemory.h"

typedef uint64_t paddress;

paddress get_Paddress(uint64_t address);
paddress grab_frame(paddress physical_address);
int get_priority_of_frame(paddress frame);


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


paddress get_Paddress(uint64_t address) {
    int prev_padd = 0;
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
