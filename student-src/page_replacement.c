#include "types.h"
#include "pagesim.h"
#include "mmu.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);

pfn_t free_frame(void) {
    pfn_t victim_pfn;
    victim_pfn = select_victim_frame();

    // TODO: evict any mapped pages.
    if (frame_table[victim_pfn].mapped) {
        vpn_t victim_vpn = frame_table[victim_pfn].vpn;

        pfn_t ptbr = (pfn_t)frame_table[victim_pfn].process->saved_ptbr;
        pte_t *page_table = (pte_t *)(mem + ptbr * PAGE_SIZE);
        pte_t *page_table_entry = &page_table[victim_vpn];

        page_table_entry->valid = 0;

        if (page_table_entry->dirty) {
            swap_write(page_table_entry, mem + victim_pfn * PAGE_SIZE);
            stats.writebacks += 1;
        }
    }

    return victim_pfn;
}

pfn_t select_victim_frame() {
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped)
        {
            return i;
        }
    }

    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t unprotected_found = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                unprotected_found = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (unprotected_found < NUM_FRAMES) {
            return unprotected_found;
        }
    }
    else if (replacement == APPROX_LRU) {
        /* Implement a LRU algorithm here */
        pfn_t pfn_min = 0;
        uint8_t min = 0xFF;
        for (uint8_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                if (frame_table[i].ref_count < min) {
                    min = frame_table[i].ref_count;
                    pfn_min = i;
                }
            }
        }
        return pfn_min;
    }
        
    
    else if (replacement == FIFO) {
        /* Implement a FIFO algorithm here */
        for (pfn_t i = 1; i < num_entries; i++) {
            pfn_t pfn = (i + last_evicted) % num_entries;
            if (!frame_table[pfn].protected) {
                last_evicted = pfn;
                return pfn;
            }
        }
    }

    // If every frame is protected, give up. This should never happen on the traces we provide you.
    panic("System ran out of memory\n");
    exit(1);
}

void daemon_update(void)
{
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    uint8_t i = 0;

    while (i < num_entries) {
        if (frame_table[i].mapped && frame_table[i].process != NULL) {
            pfn_t ptbr = frame_table[i].process->saved_ptbr;
            vpn_t vpn = frame_table[i].vpn;
            pte_t *page_table = (pte_t *)(mem + ptbr * PAGE_SIZE);

            uint8_t ref_bit = page_table[vpn].referenced << 7;
            uint8_t shifted = frame_table[i].ref_count >> 1;
            frame_table[i].ref_count = ref_bit | shifted;

            page_table[vpn].referenced = 0;
        }
        i++;
    }
}
