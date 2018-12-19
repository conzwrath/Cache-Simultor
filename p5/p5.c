#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print_helpers.h"
#include "simulator.h"

int capacity;
int block_size;
int assoc;

void printUsage() {
  printf("Usage: ./p5 [-hv] -t <tracename> -l <limit> -cache <cap> <bsize> <assoc>\n");
  printf("Options:\n");
  printf("  -h|help                         Print this help message\n");
  printf("  -v|verbose                      Optional verbose flag\n");
  printf("  -debug                          Optional debug prints $ contents\n");
  printf("  -c|cache <cap> <bsize> <assoc>  Set the cache configuration. <cap> "
        "and <bsize> are given as the log of the value.\n");
  printf("  -t|trace <tracename>            Name of trace (exclude trace.gz) \n");
  printf("  -l|limit <n>                    Simulate only first n insns \n");
  printf("\nExamples:\n");
  printf(
      "  shell>  ./p5 -v -t art-1K.trace -l 500\n");
  printf(
      "  shell>  ./p5 -verbose -trace art-full.trace \n");
  printf(
      "  shell>  ./p5 -cache 8 5 1 -debug       Creates a direct mapped cache "
      "with a capacity of 256 and block size of 32 \n");
  printf(
      "  shell>  ./p5 -cache 9 6 2 -debug       Creates a 2-way set "
      "associative cache with a capacity of 512 and block size of 64\n");
  printf(
      "  shell>  ./p5 -cache 9 7 4 -debug       Creates a 2-way set "
      "associative cache with a capacity of 512 and block size of 64\n");
}

int parse_args(char **args, int num_args, simulator_t *sim) {
  int i = 0;
  char *arg;
  bool cache_specified = false;

  // use the command line arguments to customize the simulator each run
  while (i < num_args) {
    arg = args[i++];

    // -help
    if (strcmp(arg, "-help") == 0 || strcmp(arg, "-h") == 0) {
      printUsage();
      return 0;
    }

    // -verbose
    if (strcmp(arg, "-verbose") == 0 || strcmp(arg, "-v") == 0) {
      printf("verbose mode on\n");
      sim->verbose = 1;
    }

    // -debug
    if (strcmp(arg, "-debug") == 0) {
      printf("debug mode on");
      sim->debug = 1;
    }

    // -cache C B A
    if (strcmp(arg, "-cache") == 0 || strcmp(arg, "-c") == 0) {
      if (i + 3 > num_args) {
        printf(
            "Cache description incomplete. Capacity, block size, and "
            "associativity must be specified.\nExiting...\n");
        exit(1);
      }
      int log_cap = atoi(args[i++]);
      capacity = 1 << log_cap;
      int log_block_size = atoi(args[i++]);
      block_size = 1 << log_block_size;
      assoc = atoi(args[i++]);
      if (log_cap > 25 || log_cap < 0 || log_block_size > 25 ||
          log_block_size < 0 || assoc == 0) {
        printf(
            "Cache description invalid. Capacity and block size must be "
            "between 2^0 and 2^25. Associativity must be "
            "non-zero.\nExiting...\n");
        exit(1);
      }
      if (capacity / block_size / assoc == 0) {
        printf(
            "Cache description invalid. Associativity or block size too high "
            "for given capacity.\nExiting...\n");
        exit(1);
      }
      cache_specified = true;
    }

    // -t art-1K
    if (strcmp(arg, "-trace") == 0 || strcmp(arg, "-t") == 0) {
      sim->test_trace = args[i++];
    }

    // -limit 100
    if (strcmp(arg, "-limit") == 0 || strcmp(arg, "-l") == 0) {
      sim->limit_insn = true;
      sim->insn_limit = atoi(args[i++]);
    }
  }

  if (!cache_specified) {
    printf("No cache description specified. Please use the -cache flag\n");
    exit(1);
  }

  return 1;
}

int main(int argc, char *argv[]) {
  simulator_t *sim = make_default_simulator();

  if (parse_args(argv, argc, sim)) {
    sim->cache = make_cache(capacity, block_size, assoc);
    print_simulator_header(sim);
    process_trace(sim);  // this is still where the action takes place
  }

  return 0;
}
