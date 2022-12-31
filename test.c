#include <stdio.h>
#include <stdlib.h>

#include "fastcdc.h"

int print_chunk(size_t offset, size_t len)
{
    static int chunk_id = 0;
    printf("%08zu|%08zu\n", offset, len);
    chunk_id++;
    return chunk_id;
}

int main(int argc, char *argv[]) {
  FILE *s = fopen(argv[1], "r");
  if (!s)
    exit(-1);

  printf("%8s|%8s\n", "offset", "length");
  fastcdc_stream(s, 65536 / 4, 65536, 65536 * 4, print_chunk);
  fclose(s);
  return 0;
}
