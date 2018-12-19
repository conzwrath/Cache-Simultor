#ifndef __CACHE_STATS_H
#define __CACHE_STATS_H

#include <stdbool.h>

typedef struct {
  long total_accesses;
  long total_hits;
  long total_stores;
  long total_dirty_evics;

  double hit_rate;
  long total_bytes_transf_wb;  // write-back
  long total_bytes_transf_wt;  // write-thru
} cache_stats_t;

cache_stats_t *make_cache_stats();
void calculate_stat_rates(cache_stats_t *stats, int block_size);
void update_stats(cache_stats_t *stats, bool is_hit, bool is_store,
                  bool is_dirty_evic);

#endif  // CACHE_STATS
