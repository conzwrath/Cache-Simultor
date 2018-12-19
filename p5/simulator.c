#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "simulator.h"
#include "print_helpers.h"

simulator_t *make_default_simulator() {
  simulator_t *sim = (simulator_t *)malloc(sizeof(simulator_t));

  sim->test_trace = "art-1K.trace";
  sim->verbose = 0;
  sim->debug = 0;

  sim->limit_insn = 0;
  sim->insn_limit = 0;

  return sim;
}

/*
 * Goes through the trace line by line (i.e., instruction by instruction) and
 * simulates the program being executed on a processor.
 *
 * This method is currently stripped down. It will just read in each instruction
 * one at a time, and does not access the cache.
 */
cache_stats_t *process_trace(simulator_t *sim) {
  char *line = NULL;
  // Program Stats
  long total_insns = 0;
  cache_stats_t *stats = make_cache_stats();

  printf("%s\n", sim->test_trace);

  FILE *trace = fopen(sim->test_trace, "r");
  if (trace == NULL) {
    printf("File \'%s\' not found\n", sim->test_trace);
    exit(EXIT_FAILURE);
  }
  size_t len = 0;
  size_t read;

  if (sim->debug) print_cache_header();

  while ((read = getline(&line, &len, trace)) != -1) {
    if (sim->limit_insn && total_insns == sim->insn_limit) {
      printf("Reached insn limit of %d. Ending Simulation...\n",
             sim->insn_limit);
      break;
    }
    bool is_load = line[0] == '0';
    bool is_store = line[0] == '1';
    unsigned long address = strtol(&line[2], NULL, 16);
    total_insns++;

    // Print this before you access the cache
    if (sim->debug) print_cache(sim->cache);

    // access the cache
    bool is_hit = access_cache(sim->cache, address, is_load);
    // update the cache stats
    update_stats(stats, is_hit, is_store, sim->cache->was_dirty_evic);

    // Print this after you access the cache
    if (sim->debug) print_insn_info(sim, address, is_load, is_hit);

    // prints the insn
    if (sim->verbose) printf("%s\n", line);
  }

  fclose(trace);
  if (line) free(line);

  // compute cache statistics
  calculate_stat_rates(stats, sim->cache->block_size);

  print_trace_stats(stats, total_insns);

  return stats;
}
