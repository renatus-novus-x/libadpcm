/* libadpcm.h - Minimal IOCS-based ADPCM helper for X68000
 *
 * This header provides a tiny wrapper around X68000 ADPCM IOCS calls.
 * It is intended to be header-only for simple utilities such as
 * ADPCM record and playback tools.
 */

#ifndef LIBADPCM_H
#define LIBADPCM_H

#include <x68k/iocs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long adpcm_size_t;

/* Sampling rate codes (mapped to IOCS ADPCM frequency bits). */
typedef enum {
  ADPCM_RATE_3K9  = 0, /* ~3.9 kHz  */
  ADPCM_RATE_5K2  = 1, /* ~5.2 kHz  */
  ADPCM_RATE_7K8  = 2, /* ~7.8 kHz  */
  ADPCM_RATE_10K4 = 3, /* ~10.4 kHz */
  ADPCM_RATE_15K6 = 4  /* ~15.6 kHz (typical default) */
} adpcm_rate_t;

/* Output / monitor mode (mapped to IOCS ADPCM output bits). */
typedef enum {
  ADPCM_OUT_OFF    = 0, /* no output / monitor off */
  ADPCM_OUT_LEFT   = 1, /* left only */
  ADPCM_OUT_RIGHT  = 2, /* right only */
  ADPCM_OUT_STEREO = 3  /* mono to both left and right */
} adpcm_out_t;

/* Return hardware-based sampling frequency in Hz for a given rate. */
static inline unsigned long adpcm_rate_hz(adpcm_rate_t rate)
{
  switch (rate) {
  case ADPCM_RATE_3K9:
    return 3906UL;   /* 4MHz / 1024 ->  3.906 kHz */
  case ADPCM_RATE_5K2:
    return 5208UL;   /* 4MHz / 768  ->  5.208 kHz */
  case ADPCM_RATE_7K8:
    return 7813UL;   /* 4MHz / 512  ->  7.813 kHz */
  case ADPCM_RATE_10K4:
    return 10417UL;  /* 4MHz / 384  -> 10.417 kHz */
  case ADPCM_RATE_15K6:
  default:
    return 15625UL;  /* 4MHz / 256  = 15.625 kHz */
  }
}

/* 4-bit ADPCM: 2 samples per byte, approximate size. */
static inline adpcm_size_t adpcm_bytes_for_seconds(adpcm_rate_t rate,
                                                   unsigned int seconds)
{
  unsigned long hz = adpcm_rate_hz(rate);
  unsigned long samples = hz * (unsigned long)seconds;
  return (adpcm_size_t)((samples + 1UL) / 2UL);
}

/* Internal helper: build IOCS ADPCM mode word: [10:8]=rate, [1:0]=out. */
static inline unsigned short adpcm_make_mode(adpcm_rate_t rate,
                                             adpcm_out_t  out_mode)
{
  unsigned short mode = 0;

  /* bit10-8: sampling rate code */
  mode |= (unsigned short)(((unsigned short)rate & 0x7u) << 8);

  /* bit1-0: output / monitor mode */
  mode |= (unsigned short)((unsigned short)out_mode & 0x3u);

  return mode;
}

/* Return non-zero if ADPCM device is busy (recording or playing). */
static inline int adpcm_is_busy(void)
{
  long st = _iocs_adpcmsns();
  return (st != 0L);
}

/* Force stop of ADPCM (recording or playback). */
static inline void adpcm_stop(void)
{
  /* 0: stop ADPCM input/output */
  _iocs_adpcmmod(0);
}

static inline long adpcm_start_record(void        *dst,
                                      adpcm_size_t bytes,
                                      adpcm_rate_t rate,
                                      adpcm_out_t  monitor)
{
  unsigned short mode;

  if (dst == 0 || bytes == 0UL) {
    return -1;
  }

  mode = adpcm_make_mode(rate, monitor);

  _iocs_adpcminp(dst, mode, (long)bytes);

  return (long)bytes;
}

static inline long adpcm_start_play(const void   *src,
                                    adpcm_size_t bytes,
                                    adpcm_rate_t rate,
                                    adpcm_out_t  out_mode)
{
  unsigned short mode;

  if (src == 0 || bytes == 0UL) {
    return -1;
  }

  mode = adpcm_make_mode(rate, out_mode);

  _iocs_adpcmout((void *)src, mode, (long)bytes);

  return (long)bytes;
}


/* Blocking ADPCM record using IOCS ADPCM input call.
 * Returns number of bytes written on success, or -1 on error.
 */
static inline long adpcm_record_blocking(void        *dst,
                                         adpcm_size_t bytes,
                                         adpcm_rate_t rate,
                                         adpcm_out_t  monitor)
{
  long result;

  result = adpcm_start_record(dst, bytes, rate, monitor);
  if (result < 0) {
    return result;
  }

  while (adpcm_is_busy()) {
    /* busy wait until ADPCM input is finished */
  }

  return result;
}


/* Blocking ADPCM playback using IOCS ADPCM output call.
 * Returns number of bytes played on success, or -1 on error.
 */
static inline long adpcm_play_blocking(const void   *src,
                                       adpcm_size_t  bytes,
                                       adpcm_rate_t  rate,
                                       adpcm_out_t   out_mode)
{
  long result;

  result = adpcm_start_play(src, bytes, rate, out_mode);
  if (result < 0) {
    return result;
  }

  while (adpcm_is_busy()) {
    /* busy wait until ADPCM output is finished */
  }

  return result;
}


#ifdef __cplusplus
}
#endif

#endif /* LIBADPCM_H */
