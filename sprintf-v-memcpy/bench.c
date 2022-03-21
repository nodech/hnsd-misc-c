#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hrtime.h"
#include "random.h"

#define ITERATIONS 10000
#define ITEMS 1000
#define ITEM_SIZE 29

void write_synth_sprintf(char *b32, uint8_t *name) {
  int len = strlen(b32);
  name[0] = len + 1;                          // first label length byte
  sprintf((char *)&name[1], "_%s", b32);      // base32-encoded IP data
  name[len + 2] = strlen("_synth");           // second label (TLD) length byte
  sprintf((char *)&name[len + 3], "_synth");  // pseudo-TLD, followed by 0x00
}

void write_synth_bytes(char *b32, uint8_t *name) {
  static const uint8_t synth[6] = "_synth";

  int len = strlen(b32);
  name[0] = len + 1;
  name[1] = '_';
  memcpy(&name[2], b32, len);
  name[len + 2] = 6;          // _synth
  memcpy(&name[len + 3], synth, 7);
}

void write_synth_bytes_wsize(char *b32, uint8_t *name) {
}

static char items[ITEMS][ITEM_SIZE] = {0};

void bench();

int main() {
  for (int i = 0; i < ITEMS; i++) {
    hsk_randombytes((uint8_t *)items[i], ITEM_SIZE);
    // Make characters printable.
    for (int j = 0; j < ITEM_SIZE; j++)
      items[i][j] = 65 + ((uint8_t)items[i][j] % 32);

    items[i][ITEM_SIZE - 1] = 0x00;
  }

  bench("sprintf", write_synth_sprintf);
  bench("memcpy", write_synth_bytes);

  uint8_t test_name_a[255] = {0};
  uint8_t test_name_b[255] = {0};

  write_synth_bytes(items[0], test_name_a);
  write_synth_sprintf(items[0], test_name_b);

  printf("%s\n%s\n", test_name_a, test_name_b);
}

void bench(char *name, int (fn)(char *, uint8_t *)) {
  uint64_t begin = hrtime();

  uint8_t test_name[255] = {0};

  int i, j;
  for (i = 0; i < ITERATIONS; i++) {
    for (j = 0; j < ITEMS; j++) {
      fn(items[j], test_name);
    }
  }

  uint64_t end = hrtime();
  uint64_t diff = end - begin;
  print_timediff(name, diff, ITERATIONS);
}
