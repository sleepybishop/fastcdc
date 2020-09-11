#include "fastcdc.h"
#include <math.h>

#define FASTCDC_CLAMP(x, a, b) ((x < (a)) ? (a) : ((x > b) ? b : x))
#define FASTCDC_DIVCEIL(a, b) ((a) / (b) + ((a) % (b) ? 1 : 0))
#define FASTCDC_MASK(x) (uint32_t)((1 << FASTCDC_CLAMP(x, 1, 31)) - 1);

#define AVERAGE_MIN (1 << 8)
#define AVERAGE_MAX (1 << 28)
#define MINIMUM_MIN (AVERAGE_MIN >> 2)
#define MINIMUM_MAX (AVERAGE_MAX >> 2)
#define MAXIMUM_MIN (AVERAGE_MIN << 2)
#define MAXIMUM_MAX (AVERAGE_MAX << 2)

static const uint32_t GEAR[] = {
    0x5C95C078, 0x22408989, 0x2D48A214, 0x12842087, 0x530F8AFB, 0x474536B9,
    0x2963B4F1, 0x44CB738B, 0x4EA7403D, 0x4D606B6E, 0x074EC5D3, 0x3AF39D18,
    0x726003CA, 0x37A62A74, 0x51A2F58E, 0x7506358E, 0x5D4AB128, 0x4D4AE17B,
    0x41E85924, 0x470C36F7, 0x4741CBE1, 0x01BB7F30, 0x617C1DE3, 0x2B0C3A1F,
    0x50C48F73, 0x21A82D37, 0x6095ACE0, 0x419167A0, 0x3CAF49B0, 0x40CEA62D,
    0x66BC1C66, 0x545E1DAD, 0x2BFA77CD, 0x6E85DA24, 0x5FB0BDC5, 0x652CFC29,
    0x3A0AE1AB, 0x2837E0F3, 0x6387B70E, 0x13176012, 0x4362C2BB, 0x66D8F4B1,
    0x37FCE834, 0x2C9CD386, 0x21144296, 0x627268A8, 0x650DF537, 0x2805D579,
    0x3B21EBBD, 0x7357ED34, 0x3F58B583, 0x7150DDCA, 0x7362225E, 0x620A6070,
    0x2C5EF529, 0x7B522466, 0x768B78C0, 0x4B54E51E, 0x75FA07E5, 0x06A35FC6,
    0x30B71024, 0x1C8626E1, 0x296AD578, 0x28D7BE2E, 0x1490A05A, 0x7CEE43BD,
    0x698B56E3, 0x09DC0126, 0x4ED6DF6E, 0x02C1BFC7, 0x2A59AD53, 0x29C0E434,
    0x7D6C5278, 0x507940A7, 0x5EF6BA93, 0x68B6AF1E, 0x46537276, 0x611BC766,
    0x155C587D, 0x301BA847, 0x2CC9DDA7, 0x0A438E2C, 0x0A69D514, 0x744C72D3,
    0x4F326B9B, 0x7EF34286, 0x4A0EF8A7, 0x6AE06EBE, 0x669C5372, 0x12402DCB,
    0x5FEAE99D, 0x76C7F4A7, 0x6ABDB79C, 0x0DFAA038, 0x20E2282C, 0x730ED48B,
    0x069DAC2F, 0x168ECF3E, 0x2610E61F, 0x2C512C8E, 0x15FB8C06, 0x5E62BC76,
    0x69555135, 0x0ADB864C, 0x4268F914, 0x349AB3AA, 0x20EDFDB2, 0x51727981,
    0x37B4B3D8, 0x5DD17522, 0x6B2CBFE4, 0x5C47CF9F, 0x30FA1CCD, 0x23DEDB56,
    0x13D1F50A, 0x64EDDEE7, 0x0820B0F7, 0x46E07308, 0x1E2D1DFD, 0x17B06C32,
    0x250036D8, 0x284DBF34, 0x68292EE0, 0x362EC87C, 0x087CB1EB, 0x76B46720,
    0x104130DB, 0x71966387, 0x482DC43F, 0x2388EF25, 0x524144E1, 0x44BD834E,
    0x448E7DA3, 0x3FA6EAF9, 0x3CDA215C, 0x3A500CF3, 0x395CB432, 0x5195129F,
    0x43945F87, 0x51862CA4, 0x56EA8FF1, 0x201034DC, 0x4D328FF5, 0x7D73A909,
    0x6234D379, 0x64CFBF9C, 0x36F6589A, 0x0A2CE98A, 0x5FE4D971, 0x03BC15C5,
    0x44021D33, 0x16C1932B, 0x37503614, 0x1ACAF69D, 0x3F03B779, 0x49E61A03,
    0x1F52D7EA, 0x1C6DDD5C, 0x062218CE, 0x07E7A11A, 0x1905757A, 0x7CE00A53,
    0x49F44F29, 0x4BCC70B5, 0x39FEEA55, 0x5242CEE8, 0x3CE56B85, 0x00B81672,
    0x46BEECCC, 0x3CA0AD56, 0x2396CEE8, 0x78547F40, 0x6B08089B, 0x66A56751,
    0x781E7E46, 0x1E2CF856, 0x3BC13591, 0x494A4202, 0x520494D7, 0x2D87459A,
    0x757555B6, 0x42284CC1, 0x1F478507, 0x75C95DFF, 0x35FF8DD7, 0x4E4757ED,
    0x2E11F88C, 0x5E1B5048, 0x420E6699, 0x226B0695, 0x4D1679B4, 0x5A22646F,
    0x161D1131, 0x125C68D9, 0x1313E32E, 0x4AA85724, 0x21DC7EC1, 0x4FFA29FE,
    0x72968382, 0x1CA8EEF3, 0x3F3B1C28, 0x39C2FB6C, 0x6D76493F, 0x7A22A62E,
    0x789B1C2A, 0x16E0CB53, 0x7DECEEEB, 0x0DC7E1C6, 0x5C75BF3D, 0x52218333,
    0x106DE4D6, 0x7DC64422, 0x65590FF4, 0x2C02EC30, 0x64A9AC67, 0x59CAB2E9,
    0x4A21D2F3, 0x0F616E57, 0x23B54EE8, 0x02730AAA, 0x2F3C634D, 0x7117FC6C,
    0x01AC6F05, 0x5A9ED20C, 0x158C4E2A, 0x42B699F0, 0x0C7C14B3, 0x02BD9641,
    0x15AD56FC, 0x1C722F60, 0x7DA1AF91, 0x23E0DBCB, 0x0E93E12B, 0x64B2791D,
    0x440D2476, 0x588EA8DD, 0x4665A658, 0x7446C418, 0x1877A774, 0x5626407E,
    0x7F63BD46, 0x32D2DBD8, 0x3C790F4A, 0x772B7239, 0x6F8B2826, 0x677FF609,
    0x0DC82C11, 0x23FFE354, 0x2EAC53A6, 0x16139E09, 0x0AFD0DBC, 0x2A4D4237,
    0x56A368C7, 0x234325E4, 0x2DCE9187, 0x32E8EA7E};

static uint32_t normal_size(const uint32_t mi, const uint32_t av,
                            const uint32_t len) {
  uint32_t off = mi + FASTCDC_DIVCEIL(mi, 2);
  if (off > av)
    off = av;
  uint32_t sz = av - off;
  if (sz > len)
    return len;
  return sz;
}

static uint32_t cut(const uint8_t *src, const uint32_t len, const uint32_t mi,
                    const uint32_t ma, const uint32_t ns, const uint32_t mask_s,
                    const uint32_t mask_l) {
  uint32_t n, fp = 0, i = (len < mi) ? len : mi;
  n = (ns < len) ? ns : len;
  for (; i < n; i++) {
    fp = (fp >> 1) + GEAR[src[i]];
    if (!(fp & mask_s))
      return i + 1;
  }
  n = (ma < len) ? ma : len;
  for (; i < n; i++) {
    fp = (fp >> 1) + GEAR[src[i]];
    if (!(fp & mask_l))
      return i + 1;
  }
  return i;
}

fcdc_ctx fastcdc_init(uint32_t mi, uint32_t av, uint32_t ma) {
  fcdc_ctx ctx = {0};
  ctx.mi = FASTCDC_CLAMP(mi, MINIMUM_MIN, MINIMUM_MAX);
  ctx.av = FASTCDC_CLAMP(av, AVERAGE_MIN, AVERAGE_MAX);
  ctx.ma = FASTCDC_CLAMP(ma, MAXIMUM_MIN, MAXIMUM_MAX);
  ctx.ns = normal_size(mi, av, ma);
  uint32_t bits = (uint32_t)round(log2(av));
  ctx.mask_s = FASTCDC_MASK(bits + 1);
  ctx.mask_l = FASTCDC_MASK(bits - 1);
  return ctx;
}

size_t fastcdc_update(fcdc_ctx *ctx, uint8_t *data, size_t len, int end,
                      chunk_vec *cv) {
  size_t offset = 0;
  while (((len - offset) >= ctx->ma) || (end && (offset < len))) {
    uint32_t cp = cut(data + offset, len - offset, ctx->mi, ctx->ma, ctx->ns,
                      ctx->mask_s, ctx->mask_l);
    chunk blk = {.offset = ctx->pos + offset, .len = cp};
    kv_push(chunk, *cv, blk);
    offset += cp;
  }
  ctx->pos += offset;
  return offset;
}

size_t fastcdc_stream(FILE *stream, uint32_t mi, uint32_t av, uint32_t ma,
                      chunk_vec *cv) {
  size_t offset = 0;
  int end = 0;
  fcdc_ctx cdc = fastcdc_init(mi, av, ma), *ctx = &cdc;
  size_t rs = ctx->ma * 4;
  rs = FASTCDC_CLAMP(rs, 0, UINT32_MAX);
  uint8_t *data = malloc(rs);
  while (!end) {
    size_t ar = fread(data, 1, rs, stream);
    end = feof(stream);
    offset += fastcdc_update(ctx, data, ar, end, cv);
    fseek(stream, offset, SEEK_SET);
  }
  free(data);
  return kv_size(*cv);
}
