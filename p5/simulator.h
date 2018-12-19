#ifndef __SIMULATOR_H
#define __SIMULATOR_H

#include <stdbool.h>
#include "cache.h"
#include "cache_stats.h"

typedef struct {
  char *test_trace;

  // print options, by default off
  bool verbose;
  bool debug;

  // optionally limit the simulation to the first N insns
  // do not break this functionality when you complete the code!
  bool limit_insn;
  int insn_limit;

  cache_t *cache;
} simulator_t;

simulator_t *make_default_simulator();
cache_stats_t *process_trace(simulator_t *sim);

#endif  // SIMULATOR
