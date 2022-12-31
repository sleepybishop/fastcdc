#ifndef FASTCDC_H
#define FASTCDC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint32_t mi;
  uint32_t av;
  uint32_t ma;
  uint32_t ns;
  uint32_t mask_s;
  uint32_t mask_l;
  size_t pos;
} fcdc_ctx;

#define MAP_VEC_LEN 3

typedef int(*on_chunk)(void* arg, size_t offset, size_t len);

typedef struct {
    size_t chunk_id;
    size_t chunk_len;
    uint8_t front[MAP_VEC_LEN];
    uint8_t end[MAP_VEC_LEN];
} map_entry;

fcdc_ctx fastcdc_init(uint32_t mi, uint32_t av, uint32_t ma);
size_t fastcdc_update(fcdc_ctx *ctx, uint8_t *data, size_t len, int end,
                      on_chunk cb, void *arg);
size_t fastcdc_stream(FILE *stream, uint32_t mi, uint32_t av, uint32_t ma,
                      on_chunk cb, void *arg);
#endif
