#include <stdio.h>

#include "cache.h"
#include "cache_stats.h"
#include "simulator.h"
#include "print_helpers.h"

void print_simulator_header(simulator_t *sim) {
  if (!sim->debug) {
    printf("P5 Printout for CS 3410\n");
    printf("-----------------------------------------------------\n");
  }
  printf("Trace name  \t\t\t\t%s\n", sim->test_trace);
  print_cache_config(sim->cache);
  printf("Instruction Limit\t\t\t");
  if (sim->limit_insn) {
    printf("%d\n", sim->insn_limit);
  } else {
    printf("none\n");
  }
}

/*
 * Prints out basic stats + the hit/miss rates of the cache.
 */
void print_trace_stats(cache_stats_t *stats, long total_insns) {
  printf("Processed %ld trace lines.\n", total_insns);
  printf("Num Instructions: \t\t\t%ld\n", total_insns);

  print_stats(stats);
}

void print_stats(cache_stats_t *stats) {
  printf("Cache Hit Rate = %.2f%% (Miss Rate: %.2f%%)\n",
         stats->hit_rate * 100.0, (1 - stats->hit_rate) * 100.0);
  printf("Total number of bytes transferred write-back: %ld\n",
         stats->total_bytes_transf_wb);
  printf("Total number of bytes transferred write-thru: %ld\n",
         stats->total_bytes_transf_wt);
}

void print_cache_config(cache_t *cache) {
  printf(
      "Cache size  = %dB. Each block = %dB.\n"
      "%d-way set associative cache.\n",
      cache->capacity, cache->block_size, cache->assoc);
  if (cache->n_index_bit == 0) {
    printf("This is a fully associative cache");
  }
  printf("Tag = %d bits, Index = %d bits, Offset = %d bits\n", cache->n_tag_bit,
         cache->n_index_bit, cache->n_offset_bit);
  printf(
      "There are %d sets total in the cache.\n"
      "At this associativity, that means a total of %d cache lines.\n",
      cache->n_set, cache->n_total_cache_line);
}

void print_cache_header() {
  printf(
      "[SetNum: {WayNum: Tag,cl/dirty} LRU=WayNum]\t|"
      " Block-Addr\tType\tH/M\tEvicState\n");
}

void print_cache(cache_t *cache) {
  printf("%p\n", cache);
  printf("%p\n", cache->cache_tags);
  // OPTIONAL:
  // this is NOT correct, but the formatting should help you.
  // If you would like better debugging information for your cache,
  // this is the place to add it.
  printf("[S0: {W0: %lX, C} LRU=0]\t| ", cache->cache_tags[0][0]);
}

void print_insn_info(simulator_t *sim, unsigned long address, bool is_load,
                     bool was_hit) {
  printf("%lx\t%s\t%s\t%s\n", get_cache_block_addr(sim->cache, address),
         is_load ? "L" : "S", was_hit ? "hit" : "miss",
         was_hit ? "--" : sim->cache->was_dirty_evic ? "dirty" : "clean");
}
