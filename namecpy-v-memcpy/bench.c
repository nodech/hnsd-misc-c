#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "hrtime.h"

#define MAX_NAME 255
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef struct {
  uint8_t name[MAX_NAME];
  int size;
} vector_t;

static const vector_t vectors[] = {
  { "\x02ns\x07""example\x03""com\x00", 16 },
  { "\x04name\x07""example\x03""com\x00", 18 },
  {
    "\x3f""123456789012345678901234567890123456789012345678901234567890123\x07""example\x03""com\x00",
    77
  },
  {
    "\x3f""123456789012345678901234567890123456789012345678901234567890123\x3f""123456789012345678901234567890123456789012345678901234567890123\x3f""123456789012345678901234567890123456789012345678901234567890123\x3d""1234567890123456789012345678901234567890123456789012345678901\x00",
    255
  },
  { "\x03ptr\x07""example\x03""com\x00", 17 },
  { "\x02mx\x07""example\x03""com\x00", 16 },
  { .name = "\x04""hnsd""\x09""handshake""\x03""org""\x00", .size = 20},
  { .name = "\x09""handshake""\x03""org""\x00", .size = 15},
  { .name = "\x03""org""\x00", .size = 5},
  { .name = "\x04""orgs""\x00", .size = 6},
  { .name = "\x00", .size = 1}
};

typedef struct {
  uint8_t name[MAX_NAME];
} name_t;

typedef struct {
  uint8_t name[MAX_NAME];
  uint8_t size;
} name_test_t;

int name_size(const uint8_t *nt) {
  int off = 0;

  uint8_t label = nt[0];

  while (label != 0x00) {
    off += label + 1;
    label = nt[off];
  }

  return off + 1;
}

void name_cpy(uint8_t *dest, const uint8_t *src) {
  int size = name_size(src);
  memcpy(dest, src, size);
}

void just_cpy(uint8_t *dest, const uint8_t *src) {
  memcpy(dest, src, MAX_NAME);
}

void test();
void bench();

int main() {
  /* test(); */
  bench("just_cpy", just_cpy);
  bench("name_cpy", name_cpy);
}

void bench(char *name, void (fn)(uint8_t *dest, const uint8_t *src)) {
  uint64_t begin = hrtime();
  int iters = 10000;


  name_test_t a = {0};

  volatile int i, j;
  for (i = 0; i < iters; i++) {
    for (j = 0; j < ARRAY_SIZE(vectors); j++) {
      fn(a.name, vectors[j].name);
    }
  }

  uint64_t end = hrtime();
  uint64_t diff = end - begin;

  print_timediff(name, diff, iters);
}

void test() {
  int i;
  for (i = 0; i < ARRAY_SIZE(vectors); i++) {
    int res = name_size(vectors[i].name);
    printf("test %d: Size: %d should be %d\n", i, res, vectors[i].size);
    assert(name_size(vectors[i].name) == vectors[i].size);
  }
}
