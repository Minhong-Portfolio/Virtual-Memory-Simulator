#include "stats.h"

/* The stats. See the definition in stats.h. */
stats_t stats;

void compute_stats() {
    stats.amat = ((double)MEMORY_ACCESS_TIME * stats.accesses + DISK_PAGE_WRITE_TIME * stats.writebacks + DISK_PAGE_READ_TIME * stats.page_faults) / stats.accesses;
}
