#include "VirtualMemory.h"
#include "PhysicalMemory.h"

//typedef uint64_t paddress; // for frames

typedef uint64_t vaddress; // for pages, and tree nodes

void clear_frame(word_t frame) {
    for (uint64_t i = 0; i < PAGE_SIZE; i++) {
        PMwrite(frame + i, 0);
    }
}

void VMInitialize() {
    // writes 0 on root
    clear_frame(0);
}

int get_priority_of_frame(word_t frame, word_t max_frame) {
    // case one - check if frame is empty:
//    word_t read = 0;
//    bool all_empty = true;
//    for (int i = 0; i < PAGE_SIZE; ++i) {
//        PMread(frame * PAGE_SIZE + i, &read);
//        if (read != 0) {
//            all_empty = false;
//            break;
//        }
//    }
//    if (all_empty) {
//        return 1;
//    }
    if (frame != 0) {
        return 1;
    }
        // case two - there exists an unused frame:
    else if (max_frame + 1 < NUM_FRAMES) {
        return 2;
    }
        // case three - all frames are already used:
    else {
        return 3;
    }
}


word_t dfs_rec(word_t cur_frame, word_t prev_frame, uint64_t prev_page, uint64_t current_layer,
               word_t skip_this_frame, word_t *max_frame, vaddress *page_with_max_cyclical_distance,
               word_t *frame_with_max_cyclical_distance, uint64_t *max_cyclical_distance,
               word_t *case_3_parent_frame, word_t found_frame, bool *child_is_empty, vaddress page_swapped_in) {

    *max_frame = (cur_frame > *max_frame) ? cur_frame : *max_frame;

    // recursion base, if we reached the leaves
    if (current_layer == TABLES_DEPTH) {
        uint64_t abs_page_diff = (page_swapped_in > prev_page) ? page_swapped_in - prev_page : prev_page -
                                                                                               page_swapped_in;
        uint64_t dist_from_page = (NUM_PAGES - abs_page_diff > abs_page_diff) ? abs_page_diff : NUM_PAGES -
                                                                                                abs_page_diff;
        if (dist_from_page > *max_cyclical_distance) {
            *max_cyclical_distance = dist_from_page;
            *page_with_max_cyclical_distance = prev_page;
            *case_3_parent_frame = prev_frame;
        }
        return 0;
    }

    bool the_frame_is_empty = true;
    word_t next_frame = 0;

    // branching to the next layer
    for (uint64_t offset = 0; offset < PAGE_SIZE; ++offset) {

        PMread(cur_frame * PAGE_SIZE + offset, &next_frame);

        if (next_frame != 0) {
            the_frame_is_empty = false;
            // dfs recursion call:
            word_t dfs_address = dfs_rec(next_frame, cur_frame, offset, current_layer + 1, skip_this_frame,
                                           max_frame, page_with_max_cyclical_distance, frame_with_max_cyclical_distance,
                                           max_cyclical_distance, case_3_parent_frame, found_frame, child_is_empty,
                                           page_swapped_in);

            // here it went full depth until it found a frame that points to zero
            if (dfs_address != 0 && found_frame == 0) {
                if (*child_is_empty) {
                    PMwrite(cur_frame * PAGE_SIZE + offset, 0); // writing to the parent 0 to unlink
                    *child_is_empty = false;
                }
                found_frame = dfs_address;
                break;
            }
        }
    }

    // case: entered an empty frame
    if (the_frame_is_empty && cur_frame != 0) {

        // solution: return the previous frame, so when the dfs will enter
        if (cur_frame != skip_this_frame) {
            *child_is_empty = true;
            return cur_frame;
        }
    }

    return found_frame;
}


word_t dfs(word_t skip_this_frame, word_t *max_frame, vaddress *page_with_max_cyclical_distance,
             word_t *frame_with_max_cyclical_distance, word_t *case_3_parent_frame, vaddress page_swapped_in) {
    uint64_t max_c_d = 0;
    bool empty_child = false;
    return dfs_rec(0, 0, 0, 0, skip_this_frame, max_frame,
                   page_with_max_cyclical_distance, frame_with_max_cyclical_distance,
                   &max_c_d, case_3_parent_frame, 0,
                   &empty_child, page_swapped_in);
}

word_t grab_frame(word_t skip_this_frame, vaddress page_swapped_in) {
    word_t max_frame = 0;
    vaddress page_with_max_cyclical_distance = 0;
    word_t frame_with_max_cyclical_distance = 0;
    word_t case_3_parent_frame = 0;
    word_t chosen_frame = dfs(skip_this_frame, &max_frame, &page_with_max_cyclical_distance,
                                &frame_with_max_cyclical_distance,
                                &case_3_parent_frame, page_swapped_in);

    switch (get_priority_of_frame(chosen_frame, max_frame)) {
        case 1:
            // remove the references from its parents (the dfs does it by itself TODO check this)
            return chosen_frame;
        case 2:
            return max_frame + 1;
        case 3:
            // and evicting it
            PMevict(frame_with_max_cyclical_distance, page_with_max_cyclical_distance);
            // TODO check if we need to do something to that frame before returning it
            return frame_with_max_cyclical_distance;
    }
    return -1;
}

word_t get_Paddress(vaddress Vaddress) {

    word_t last_padd = 0;
    word_t cur_padd = 0;

    for (uint64_t offsets = 0; offsets < TABLES_DEPTH; offsets++) {

        uint64_t sh_left = (UINT64_WIDTH - (TABLES_DEPTH + 1 + offsets) * OFFSET_WIDTH);
        uint64_t sh_right = (UINT64_WIDTH - OFFSET_WIDTH);
        uint64_t rel_vadd = (Vaddress << sh_left) >> sh_right;

        PMread(cur_padd * PAGE_SIZE + rel_vadd, &cur_padd);

        if (cur_padd == 0) {

            word_t frame = grab_frame(cur_padd, Vaddress);

            if (offsets + 1 == TABLES_DEPTH) {
                // then we reached the leaves
                PMrestore(frame, Vaddress);
            } else {
                // zero out the table
                clear_frame(frame);
            }

            PMwrite(last_padd * PAGE_SIZE + rel_vadd, frame);
            cur_padd = frame;
        }
        last_padd = cur_padd;
    }

    return cur_padd;
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