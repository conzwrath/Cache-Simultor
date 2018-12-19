#ifndef __PRINT_HELPERS_H
#define __PRINT_HELPERS_H

#include <stdbool.h>
#include "cache.h"
#include "cache_stats.h"
#include "simulator.h"

void print_simulator_header(simulator_t *sim);

void print_insn_info(simulator_t *sim, unsigned long address, bool is_load, bool was_hit);
void print_trace_stats(cache_stats_t *stats, long total_insns);

void print_stats(cache_stats_t *stats);

void print_cache_config(cache_t *cache);
void print_cache_header();
void print_cache(cache_t *cache);

#endif  // PRINT_HELPERS
