#include <stdlib.h>

#include "cache_stats.h"

/* For Project 5 you will consider a variety of cache configurations.
 * For each cache you configure & simulate, you will maintain the statistics
 * for that cache with an instance of this struct
 */
cache_stats_t *make_cache_stats() {
  cache_stats_t *stats = (cache_stats_t *)malloc(sizeof(cache_stats_t));

  stats->total_accesses = 0;
  stats->total_hits = 0;
  stats->total_stores = 0;
  stats->total_dirty_evics = 0;

  stats->hit_rate = 0.0;
  stats->total_bytes_transf_wb = 0;
  stats->total_bytes_transf_wt = 0;

  return stats;
}

void update_stats(cache_stats_t *stats, bool is_hit, bool is_store,
                  bool is_dirty_evic) {
  if (is_hit) stats->total_hits++;
  if (is_store) stats->total_stores++;
  if (is_dirty_evic) stats->total_dirty_evics++;

  stats->total_accesses++;
}

// could do this in the previous method, but that's a lot of extra divides...
void calculate_stat_rates(cache_stats_t *stats, int block_size) {
  stats->hit_rate = stats->total_hits / (double)stats->total_accesses;
  // FIX THIS CODE!
  // you will need to modify this function in order to properly
  // calculate wb and wt data
  int traffic_in = block_size * (stats->total_accesses - stats->total_hits);
  stats->total_bytes_transf_wb = traffic_in + (block_size * (stats->total_dirty_evics));
  stats->total_bytes_transf_wt = traffic_in + (4 * (stats->total_stores));
  
}
