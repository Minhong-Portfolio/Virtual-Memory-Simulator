#pragma once

#include "mmu.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
 
static inline vpn_t get_vaddr_vpn(vaddr_t addr) {
    return (vpn_t)(addr >> OFFSET_LEN);
}

static inline uint16_t get_vaddr_offset(vaddr_t addr) {
    return (uint16_t)(addr & ((1 << OFFSET_LEN) - 1));
}

static inline pte_t* get_page_table(pfn_t ptbr, uint8_t *memory) {
    pte_t *page_table = (pte_t *)(memory + ptbr * PAGE_SIZE);
    return page_table;
}

static inline pte_t* get_page_table_entry(vpn_t vpn, pfn_t ptbr, uint8_t *memory) {
    pte_t *page_table = get_page_table(ptbr, memory);
    return &page_table[vpn];
}

static inline paddr_t get_physical_address(pfn_t pfn, uint16_t offset) {
    paddr_t physical_address = (paddr_t)(pfn * PAGE_SIZE + offset);
    return physical_address;
}

#pragma GCC diagnostic pop
