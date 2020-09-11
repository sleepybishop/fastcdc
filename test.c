#include <stdio.h>
#include <stdlib.h>

#include "fastcdc.h"

int main(int argc, char *argv[]) {
  chunk_vec cv;
  FILE *s = fopen(argv[1], "r");
  if (!s)
    exit(-1);
  kv_init(cv);
  fastcdc_stream(s, 65536 / 4, 65536, 65536 * 4, &cv);
  printf("%8s|%8s\n", "offset", "length");
  for (int i = 0; i < kv_size(cv); i++)
    printf("%08lu|%08lu\n", kv_A(cv, i).offset, kv_A(cv, i).len);
  kv_destroy(cv);
  fclose(s);
  return 0;
}
