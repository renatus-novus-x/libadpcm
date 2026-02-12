/* adpcmrec.c - Simple ADPCM recorder using libadpcm.h
 *
 * Usage:
 *   adpcmrec filename seconds [rate]
 *
 *   rate:
 *     0 = 3.9kHz
 *     1 = 5.2kHz
 *     2 = 7.8kHz
 *     3 = 10.4kHz
 *     4 = 15.6kHz (default)
 */

#include <stdio.h>
#include <stdlib.h>

#include "libadpcm.h"

static void print_usage(const char *prog)
{
  printf("Usage: %s filename seconds [rate]\n", prog);
  printf("  rate: 0=3.9kHz 1=5.2kHz 2=7.8kHz 3=10.4kHz 4=15.6kHz (default 4)\n");
}

static adpcm_rate_t parse_rate(const char *s)
{
  int r = atoi(s);
  if (r <= 0) {
    return ADPCM_RATE_3K9;
  } else if (r == 1) {
    return ADPCM_RATE_5K2;
  } else if (r == 2) {
    return ADPCM_RATE_7K8;
  } else if (r == 3) {
    return ADPCM_RATE_10K4;
  } else {
    return ADPCM_RATE_15K6;
  }
}

int main(int argc, char *argv[])
{
  const char    *filename;
  unsigned int   seconds;
  adpcm_rate_t   rate = ADPCM_RATE_15K6;
  adpcm_size_t   bytes;
  void          *buffer;
  long           rec_bytes;
  FILE          *fp;

  if (argc < 3) {
    print_usage(argv[0]);
    return 1;
  }

  filename = argv[1];
  seconds = (unsigned int)atoi(argv[2]);
  if (seconds == 0) {
    printf("seconds must be >= 1\n");
    return 1;
  }

  if (argc >= 4) {
    rate = parse_rate(argv[3]);
  }

  bytes = adpcm_bytes_for_seconds(rate, seconds);
  buffer = malloc(bytes);
  if (buffer == NULL) {
    printf("malloc(%lu) failed\n", (unsigned long)bytes);
    return 1;
  }

  printf("Recording %u second(s), approx %lu bytes, rate=%lu Hz...\n",
         seconds,
         (unsigned long)bytes,
         adpcm_rate_hz(rate));
  printf("Starting non-blocking record. Speak into the microphone.\n");

  rec_bytes = adpcm_start_record(buffer, bytes, rate, ADPCM_OUT_STEREO);
  if (rec_bytes < 0) {
    printf("adpcm_start_record() failed\n");
    free(buffer);
    return 1;
  }

  printf("Recording... (polling adpcm_is_busy(), do other work here)\n");
  while (adpcm_is_busy()) {
    /* TODO: handle other tasks such as communication here. */
  }
  printf("Recording finished.\n");

  fp = fopen(filename, "wb");
  if (fp == NULL) {
    printf("fopen('%s','wb') failed\n", filename);
    free(buffer);
    return 1;
  }

  if (fwrite(buffer, 1, (size_t)rec_bytes, fp) != (size_t)rec_bytes) {
    printf("fwrite() failed (short write)\n");
    fclose(fp);
    free(buffer);
    return 1;
  }

  fclose(fp);
  free(buffer);

  printf("Done. Wrote %ld bytes to '%s'.\n", rec_bytes, filename);
  return 0;
}
