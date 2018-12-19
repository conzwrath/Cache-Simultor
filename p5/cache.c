#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "cache.h"

// FIX THIS CODE!
void **make_2d_matrix(int n_row, int n_col, size_t size) {
  void **matrix = NULL;

  // malloc an array with n_rows
  // for each element in the array, malloc another array with n_col
  matrix = (void*)malloc(n_row * sizeof(void*));
  for (int i = 0; i < n_row; i++) {
      matrix[i] = malloc(n_col * (size));
  }

  return matrix;
}

cache_t *make_cache(int capacity, int block_size, int assoc) {
  cache_t *cache = (cache_t *)malloc(sizeof(cache_t));

  cache->capacity = capacity;      // in Bytes
  cache->block_size = block_size;  // in Bytes
  cache->assoc = assoc;            // 1, 2, 3... etc.

  cache->was_dirty_evic = false;

  // FIX THIS CODE!
  // first, correctly set these 5 variables. THEY ARE ALL WRONG
  // note: you may find math.h's log2 function useful
  cache->n_total_cache_line = capacity/block_size;
  cache->n_set = (cache->n_total_cache_line)/assoc;
  cache->n_offset_bit =log2(block_size);
  cache->n_index_bit = log2(cache->n_set);
  cache->n_tag_bit = 32 - (cache->n_index_bit) - (cache->n_offset_bit);

  // next create the cache tags
  // Note; this is also incorrect (it shouldn't be a 1 x 1 cache)
  // FIX THIS CODE!

  cache->cache_tags = (unsigned long **)make_2d_matrix(cache->n_set, assoc, sizeof(unsigned long));
  cache->dirty_bits = (bool **)make_2d_matrix(cache->n_set, assoc, sizeof(bool));
  cache->lru_way = (int *)malloc(cache->n_set * sizeof(int));

  // initializes cache tags to 0, dirty bits to false, and LRU bits to 0
  // this code is correct EXCEPT the for-loops should not stop at [1][1]
  // FIX THIS CODE!
  for (int i = 0; i < cache->n_set; i++) {
    for (int j = 0; j < assoc; j++) {
      cache->cache_tags[i][j] = 0;
      cache->dirty_bits[i][j] = 0;
    }
    cache->lru_way[i] = 0;
  }

  return cache;
}

unsigned long get_cache_tag(cache_t *cache, unsigned long addr) {
  // FIX THIS CODE!
  unsigned long andselector = 0b11111111111111111111111111111111;
  unsigned long result;
  andselector = andselector >> (32 - cache->n_tag_bit);
  result = (addr >> (cache->n_offset_bit + cache->n_index_bit)) & andselector;
  return result;
}

unsigned long get_cache_index(cache_t *cache, unsigned long addr) {
  // FIX THIS CODE!
  unsigned long andselector = 0b11111111111111111111111111111111;
  unsigned long result;
  andselector = andselector >> (32 - cache->n_index_bit);
  result = (addr >> cache->n_offset_bit) & andselector;
  return result;
}

unsigned long get_cache_block_addr(cache_t *cache, unsigned long addr) {
  // FIX THIS CODE!
  unsigned long andselector = 0b11111111111111111111111111111111;
  unsigned long result;
  andselector = andselector << cache->n_offset_bit;
  result = addr & andselector;
  return result;
}


/* this function takes a cache, an address, and a flag as to whether the 
 * access is a load or store.
 * functionality in no particular order:
 * 	(1) look up the address in the cache to determine if this access is
 *      a hit or a miss.
 * 	(2) update the cache_tags if necessary to reflect the new state of the
 *      cache if something was brought into the cache.
 * 	(3) update the dirty_bits if we just made a cache line dirty (stored a value to it)
 *      and set the was_dirty_evic flag accordingly if we evicted a dirty cache line.
 * 	(4) update the lru_way field by making the appropriate call to update_cache_lru().
 * Return true if there was a hit, false if there was a miss.
 * IMPORTANT: Use the "get" helper functions above. They will make your life easier.
 */
bool access_cache(cache_t *cache, unsigned long addr, bool is_load) {
  // FIX THIS CODE!

  cache->was_dirty_evic = false;
  bool cachehit = false;
  int count = 0;
  if (is_load == true) {
    while (count < cache->assoc && cachehit == false) {
      if (cache->cache_tags[get_cache_index(cache, addr)][count] == get_cache_tag(cache, addr)) {
        cachehit = true;
        update_cache_lru(cache, get_cache_index(cache, addr), count);
        count = count + 1;
      }
      else {
        count = count + 1;
      }
    }
    if (cachehit == false) {
      int way = cache->lru_way[get_cache_index(cache, addr)];
      int set = get_cache_index(cache, addr);
      cache->was_dirty_evic = cache->dirty_bits[set][way];
      cache->cache_tags[set][way] =
        get_cache_tag(cache, addr);
      
      update_cache_lru(cache, set, way);
    }
  }
  else if (is_load == false) {
    while (count < cache->assoc && cachehit == false) {
      if (cache->cache_tags[get_cache_index(cache, addr)][count] == get_cache_tag(cache, addr)) {
        int set = get_cache_index(cache, addr);
        cachehit = true;
        update_cache_lru(cache, set, count);
        count = count + 1;
      }
      else {
        count = count + 1;
      }
    }
    if (cachehit == false) {
      int way = cache->lru_way[get_cache_index(cache, addr)];
      int set = get_cache_index(cache, addr);
      cache->was_dirty_evic = cache->dirty_bits[set][way];
      cache->dirty_bits[set][way] = true;
      cache->cache_tags[set][way] =
        get_cache_tag(cache, addr);
      update_cache_lru(cache, set, way);
      
    }
  }
  return cachehit;
}

/*
 * LRU cannot be maintained with a single counter if there are
 * more than 2 ways. So we'll just use an approximation:
 * 	If there is just one way, the LRU bit is always 0.
 * 	If there are two ways, the LRU bit is always the way you DIDN'T just touch.
 *  If there are more than 2 ways, the LRU bit is always 1 higher (w/wrap-around)
 *  	than the way you just touched.
 * For example, if there are 4 ways, and you touch way 0, then the new LRU should be 1.
 * 		If you touch way 3, the new LRU should be 0.
 * the_set: identifies the set in the cache we're talking about
 * touched_way: identifies the way we just touched.
 */
void update_cache_lru(cache_t *cache, int the_set, int touched_way) {
  // LRU remains 0
  if (cache->assoc == 1) return;

  if (touched_way < (cache->assoc - 1)) {
    cache->lru_way[the_set] = touched_way + 1;
  } else {
    cache->lru_way[the_set] = 0;
  }
}
