#ifndef FASTCDC_H
#define FASTCDC_H

#include "kvec.h"
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

typedef struct {
  size_t offset;
  size_t len;
} chunk;

typedef kvec_t(chunk) chunk_vec;

fcdc_ctx fastcdc_init(uint32_t mi, uint32_t av, uint32_t ma);
size_t fastcdc_update(fcdc_ctx *ctx, uint8_t *data, size_t len, int end,
                      chunk_vec *cv);
size_t fastcdc_stream(FILE *stream, uint32_t mi, uint32_t av, uint32_t ma,
                      chunk_vec *cv);
#endif
