#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "arena.h"

void mult_8_bytes(uintptr_t addr) {
  if((addr & 0x7) != 0) {
    fprintf(stderr, "The address is not a multiple of 8 bytes (0x%lx)", addr);
    exit(-1);
  }
}

void test_memory(int p_flag, Arena *arena) {
  size_t prev_used = 0;
  for(int i = 0; i < 80; i++) {
    size_t sz = rand() % 80 + 1;
    void* addr = arena_alloc(arena, sz);
    uintptr_t ptr_addr = (uintptr_t) addr;
    size_t used_now = arena->begin->used;

    if(p_flag) {
      printf("[Iteration] %d\t[Size requested] %ld\t[Pointer] %p\t [Allocated] %ld\n", i, sz, addr, used_now - prev_used);
    }
    
    mult_8_bytes(ptr_addr);
    prev_used = used_now;
  }
}

size_t count_memory(Arena *arena) {
  size_t count = 0;
  Region *region = arena->begin;
  
  while(region != NULL) {
    count += region->used;
    region = region->next;
  }
  return count;
}

size_t count_regions(Arena *arena) {
  size_t cnt_regions = 0;
  Region *region = arena->begin;
  
  while(region != NULL) {
    cnt_regions += 1;
    region = region->next;
  }
  return cnt_regions;
}

int main(void) {
  Arena arena;
  arena_init(&arena);
  
  srand(time(NULL));
  test_memory(0, &arena);
  assert(arena.begin == arena.end && "The region should be the same one since we allocate 8KB");
  test_memory(0, &arena);
  assert(count_regions(&arena) <= 2 && count_regions(&arena) >= 1 && "The count of regions should be 1 or 2");

  arena_reset(&arena);
  assert(count_memory(&arena) == 0 && "After reset the memory used is different from 0!");

  // Since I have reset the memory I reallocate other 2 pages
  test_memory(0, &arena);
  test_memory(0, &arena);
  test_memory(0, &arena);
  assert(count_regions(&arena) <= 3 && count_regions(&arena) >= 1 && "The count of regions should be 1 or 2 or 3");

  arena_free(&arena);
  // Try to run valgrind, to check for memory issues:
  // [EXPECTED RESULT]: All heap blocks were freed -- no leaks are possible
  
  return 0;
}
