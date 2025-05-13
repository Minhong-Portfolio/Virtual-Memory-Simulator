#include "proc.h"
#include "mmu.h"
#include "pagesim.h"
#include "address_splitting.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void proc_init(pcb_t *proc) {
    // TODO: initialize proc's page table.
    pfn_t freed_frame = free_frame();
    proc->saved_ptbr = freed_frame;

    frame_table[freed_frame].protected = 1;
    frame_table[freed_frame].mapped = 0;
    frame_table[freed_frame].ref_count = 0;
    frame_table[freed_frame].process = proc;
    frame_table[freed_frame].vpn = 0; // we set a frame for page table since this is how we do it in the project
    memset(mem + freed_frame*PAGE_SIZE, 0, PAGE_SIZE);
}

void context_switch(pcb_t *proc) {
    // TODO: update any global vars and proc's PCB to match the context_switch.
    current_process = proc;
    PTBR = proc->saved_ptbr;
    proc->state = PROC_RUNNING;
}

void proc_cleanup(pcb_t *proc) {
    // TODO: Iterate the proc's page table and clean up each valid page
    pfn_t ptbr = proc->saved_ptbr;
    pte_t *page_table = (pte_t *)(mem + PAGE_SIZE * ptbr);
    size_t index = 0;

    while (index < NUM_PAGES) {
        pte_t *curr_entry = &page_table[index];

        if (curr_entry->valid != 0) {
            pfn_t frame = curr_entry->pfn;
            frame_table[frame].protected = 0;
            frame_table[frame].mapped = 0;
        }

        if (swap_exists(curr_entry)) {
            swap_free(curr_entry);
        }

        index++;
    }

    frame_table[ptbr].protected = 0;
    frame_table[ptbr].mapped = 0;

}

#pragma GCC diagnostic pop
