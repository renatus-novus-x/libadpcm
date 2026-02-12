/* adpcmplay.c - Simple ADPCM player using libadpcm.h
 *
 * Usage:
 *   adpcmplay filename [rate] [out]
 *
 *   rate:
 *     0 = 3.9kHz
 *     1 = 5.2kHz
 *     2 = 7.8kHz
 *     3 = 10.4kHz
 *     4 = 15.6kHz (default)
 *
 *   out:
 *     0 = off
 *     1 = left
 *     2 = right
 *     3 = stereo (default)
 */

#include <stdio.h>
#include <stdlib.h>

#include "libadpcm.h"

static void print_usage(const char *prog)
{
  printf("Usage: %s filename [rate] [out]\n", prog);
  printf("  rate: 0=3.9kHz 1=5.2kHz 2=7.8kHz 3=10.4kHz 4=15.6kHz (default 4)\n");
  printf("  out : 0=off 1=left 2=right 3=stereo (default 3)\n");
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

static adpcm_out_t parse_out(const char *s)
{
  int o = atoi(s);
  if (o <= 0) {
    return ADPCM_OUT_OFF;
  } else if (o == 1) {
    return ADPCM_OUT_LEFT;
  } else if (o == 2) {
    return ADPCM_OUT_RIGHT;
  } else {
    return ADPCM_OUT_STEREO;
  }
}

int main(int argc, char *argv[])
{
  const char   *filename;
  FILE         *fp;
  long          file_size;
  void         *buffer;
  adpcm_rate_t  rate = ADPCM_RATE_15K6;
  adpcm_out_t   out  = ADPCM_OUT_STEREO;
  long          played;

  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }

  filename = argv[1];

  if (argc >= 3) {
    rate = parse_rate(argv[2]);
  }
  if (argc >= 4) {
    out = parse_out(argv[3]);
  }

  fp = fopen(filename, "rb");
  if (fp == NULL) {
    printf("fopen('%s','rb') failed\n", filename);
    return 1;
  }

  if (fseek(fp, 0L, SEEK_END) != 0) {
    printf("fseek() failed\n");
    fclose(fp);
    return 1;
  }

  file_size = ftell(fp);
  if (file_size <= 0) {
    printf("ftell() failed or file too small\n");
    fclose(fp);
    return 1;
  }

  if (fseek(fp, 0L, SEEK_SET) != 0) {
    printf("fseek() failed\n");
    fclose(fp);
    return 1;
  }

  buffer = malloc((adpcm_size_t)file_size);
  if (buffer == NULL) {
    printf("malloc(%ld) failed\n", file_size);
    fclose(fp);
    return 1;
  }

  if (fread(buffer, 1, (size_t)file_size, fp) != (size_t)file_size) {
    printf("fread() failed (short read)\n");
    fclose(fp);
    free(buffer);
    return 1;
  }

  fclose(fp);

  printf("Playing '%s' (%ld bytes), rate=%lu Hz, out=%d...\n",
         filename,
         file_size,
         adpcm_rate_hz(rate),
         (int)out);

  printf("Starting non-blocking playback.\n");

  played = adpcm_start_play(buffer,
                            (adpcm_size_t)file_size,
                            rate,
                            out);
  if (played < 0) {
    printf("adpcm_start_play() failed\n");
    free(buffer);
    return 1;
  }

  printf("Playing... (polling adpcm_is_busy(), do other work here)\n");
  while (adpcm_is_busy()) {
    /* TODO: handle other tasks such as communication here. */
  }
  printf("Playback finished.\n");

  free(buffer);
  printf("Done.\n");
  return 0;
}
