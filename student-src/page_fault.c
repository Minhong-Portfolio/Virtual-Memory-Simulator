#include "mmu.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void page_fault(vaddr_t address) {
    // TODO: Get a new frame, then correctly update the page table and frame table

    vpn_t VPN = get_vaddr_vpn(address);
    pte_t *page_table = (pte_t *)(mem + PAGE_SIZE * PTBR);
    pte_t *page_table_entry = &page_table[VPN];

    pfn_t PFN = free_frame();
    uint32_t *new_frame = (uint32_t *)(mem + PAGE_SIZE * PFN);

    if (swap_exists(page_table_entry)) {
        swap_read(page_table_entry, new_frame);
    } else {
        for (size_t i = 0; i < PAGE_SIZE / sizeof(uint32_t); i++) {
            new_frame[i] = 0;
        }
    }

    page_table_entry->valid = 1;
    page_table_entry->dirty = 0;
    page_table_entry->pfn = PFN;

    fte_t *frame_table_entry = &frame_table[PFN];

    frame_table_entry->mapped = 1;
    frame_table_entry->process = current_process;
    frame_table_entry->vpn = VPN;
    frame_table_entry->ref_count = 0;

    stats.page_faults += 1;

}

#pragma GCC diagnostic pop
