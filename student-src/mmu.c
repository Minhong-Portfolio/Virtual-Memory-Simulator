#include "mmu.h"
#include "pagesim.h"
#include "address_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/* The frame table pointer. You will set this up in system_init. */
fte_t *frame_table;

void system_init(void) {
    // TODO: initialize the frame_table pointer.
    frame_table = (fte_t*)mem;
    for(int i = 0; i < NUM_FRAMES; i++) {
        frame_table[i].protected = 0;
        frame_table[i].mapped = 0;
        frame_table[i].ref_count = 0;
        frame_table[i].process = NULL;
        frame_table[i].vpn = 0;
    }
    frame_table[0].protected = 1;
}

uint8_t mem_access(vaddr_t address, char access, uint8_t data) {
    // TODO: translate virtual address to physical, then perform the specified operation

    vpn_t VPN = get_vaddr_vpn(address);
    uint16_t offset = get_vaddr_offset(address);

    pte_t *page_table = (pte_t *)(mem + PTBR * PAGE_SIZE);
    pte_t *page_table_entry = &page_table[VPN];

    if(page_table_entry->valid == 0) {
        page_fault(address);
    }

    page_table_entry->referenced = 1;
    pfn_t pfn = page_table_entry->pfn;

    stats.accesses += 1;

    // Either read or write the data to the physical address depending on 'rw'
    if (access == 'r') {
        return mem[get_physical_address(pfn, offset)];
    } else {
        page_table_entry->dirty = 1;
        mem[get_physical_address(pfn, offset)] = data;
        return data;
    }

    return 0;
}
