#ifndef __CACHE_H
#define __CACHE_H

#include <stdbool.h>
#include <stdlib.h>

#define ADDRESS_SIZE 32  // in bits

typedef struct {
  int capacity;    // in Bytes
  int block_size;  // in Bytes
  int assoc;       // 1 for 1-way set associative, etc.

  // Modify the constructor to properly initialize these variables
  int n_set;
  int n_total_cache_line;
  int n_offset_bit;
  int n_index_bit;
  int n_tag_bit;

  // tags are big numbers, store them as longs
  // 2D Arrays: first dimension = which set, second dimension = which way
  // (at first, begin with a direct mapped cache; use [0] for the second
  // dimension)
  unsigned long **cache_tags;
  bool **dirty_bits;

  // only 1 dimension b/c LRU field is for the entire set
  // ignore this until you begin support for the n-way set associative cache
  int *lru_way;

  // whether the most recent cache access resulted in a dirity eviction
  bool was_dirty_evic;
} cache_t;

void **make_2d_matrix(int n_row, int n_col, size_t size);
cache_t *make_cache(int capacity, int block_size, int assoc);
unsigned long get_cache_tag(cache_t *cache, unsigned long addr);
unsigned long get_cache_index(cache_t *cache, unsigned long addr);
unsigned long get_cache_block_addr(cache_t *cache, unsigned long addr);
bool access_cache(cache_t *cache, unsigned long addr, bool is_load);
void update_cache_lru(cache_t *cache, int the_set, int touched_way);

#endif  // CACHE
