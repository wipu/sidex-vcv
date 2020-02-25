#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.hpp>
#include "sysexconverter.hpp"

void convertAndDump(SidExConverter *conv) {
  conv->convert();

  printf("sid:      ");
  // TODO more padding for header and footer
  for(int i=0; i<NUM_SID_REGISTERS; i++) {
    printf(" %02X", conv->sid[i]);
    if(i % 7 == 6) {
      // space for the MSB, to align with sysex
      printf("   ");
    }
  }
  printf("\n");
  printf("syx:");
  for(int i=0; i<SYSEX_DUMP_LENGTH; i++) {
    printf(" %02X", conv->out[i]);
  }
  printf("\n");
}

void handleCommand(char* name, double value, SidExConverter *conv) {
  fprintf(stderr, "* %s    %f\n", name, value);
  if(strcmp("convert\n", name) == 0) {
    convertAndDump(conv);
    return;
  }
  if(strcmp("gate1\n", name) == 0) {
    conv->gate1 = value;
    return;
  }
  if(strcmp("gate2\n", name) == 0) {
    conv->gate2 = value;
    return;
  }
  if(strcmp("gate3\n", name) == 0) {
    conv->gate3 = value;
    return;
  }
  if(strcmp("freq1\n", name) == 0) {
    conv->freq1 = value;
    return;
  }
  if(strcmp("freq2\n", name) == 0) {
    conv->freq2 = value;
    return;
  }
  if(strcmp("freq3\n", name) == 0) {
    conv->freq3 = value;
    return;
  }
  if(strcmp("a1\n", name) == 0) {
    conv->a1 = value;
    return;
  }
  if(strcmp("d1\n", name) == 0) {
    conv->d1 = value;
    return;
  }
  if(strcmp("s1\n", name) == 0) {
    conv->s1 = value;
    return;
  }
  if(strcmp("r1\n", name) == 0) {
    conv->r1 = value;
    return;
  }
  if(strcmp("a2\n", name) == 0) {
    conv->a2 = value;
    return;
  }
  if(strcmp("d2\n", name) == 0) {
    conv->d2 = value;
    return;
  }
  if(strcmp("s2\n", name) == 0) {
    conv->s2 = value;
    return;
  }
  if(strcmp("r2\n", name) == 0) {
    conv->r2 = value;
    return;
  }
  if(strcmp("a3\n", name) == 0) {
    conv->a3 = value;
    return;
  }
  if(strcmp("d3\n", name) == 0) {
    conv->d3 = value;
    return;
  }
  if(strcmp("s3\n", name) == 0) {
    conv->s3 = value;
    return;
  }
  if(strcmp("r3\n", name) == 0) {
    conv->r3 = value;
    return;
  }
  if(strcmp("wf_tri_1\n", name) == 0) {
    conv->wf_tri_1 = value;
    return;
  }
  if(strcmp("wf_saw_1\n", name) == 0) {
    conv->wf_saw_1 = value;
    return;
  }
  if(strcmp("wf_pul_1\n", name) == 0) {
    conv->wf_pul_1 = value;
    return;
  }
  if(strcmp("wf_noi_1\n", name) == 0) {
    conv->wf_noi_1 = value;
    return;
  }
  if(strcmp("wf_tri_2\n", name) == 0) {
    conv->wf_tri_2 = value;
    return;
  }
  if(strcmp("wf_saw_2\n", name) == 0) {
    conv->wf_saw_2 = value;
    return;
  }
  if(strcmp("wf_pul_2\n", name) == 0) {
    conv->wf_pul_2 = value;
    return;
  }
  if(strcmp("wf_noi_2\n", name) == 0) {
    conv->wf_noi_2 = value;
    return;
  }
  if(strcmp("wf_tri_3\n", name) == 0) {
    conv->wf_tri_3 = value;
    return;
  }
  if(strcmp("wf_saw_3\n", name) == 0) {
    conv->wf_saw_3 = value;
    return;
  }
  if(strcmp("wf_pul_3\n", name) == 0) {
    conv->wf_pul_3 = value;
    return;
  }
  if(strcmp("wf_noi_3\n", name) == 0) {
    conv->wf_noi_3 = value;
    return;
  }
  if(strcmp("cutoff\n", name) == 0) {
    conv->cutoff = value;
    return;
  }
  if(strcmp("reso\n", name) == 0) {
    conv->reso = value;
    return;
  }
  if(strcmp("filt1\n", name) == 0) {
    conv->filt1 = value;
    return;
  }
  if(strcmp("filt2\n", name) == 0) {
    conv->filt2 = value;
    return;
  }
  if(strcmp("filt3\n", name) == 0) {
    conv->filt3 = value;
    return;
  }
  if(strcmp("filtExt\n", name) == 0) {
    conv->filtExt = value;
    return;
  }
  if(strcmp("filter_mode\n", name) == 0) {
    conv->filter_mode = value;
    return;
  }
  if(strcmp("master_volume\n", name) == 0) {
    conv->master_volume = value;
    return;
  }
  if(strcmp("pw1\n", name) == 0) {
    conv->pw1 = value;
    return;
  }
  if(strcmp("pw2\n", name) == 0) {
    conv->pw2 = value;
    return;
  }
  if(strcmp("pw3\n", name) == 0) {
    conv->pw3 = value;
    return;
  }
  fprintf(stderr, "Unsupported command: %s\n", name);
}

void zero_all_values(SidExConverter *conv) {
  fprintf(stderr, "* zeroing all inputs\n");
  conv->gate1 = 0.0;
  conv->gate2 = 0.0;
  conv->gate3 = 0.0;
  conv->freq1 = 0.0;
  conv->freq2 = 0.0;
  conv->freq3 = 0.0;
  conv->master_volume = 0.0;
}

int main(int argc, char** argv) {
  SidExConverter conv;

  size_t in_buf_len = 1024;
  char *in_buf = (char *) malloc(in_buf_len);
  int line_len;
  bool reading_name = true;
  char *name = (char *) malloc(in_buf_len);
  char *strtod_ptr;

  while(true) {
    line_len = getline(&in_buf, &in_buf_len, stdin);
    if(line_len <= 0) {
      fprintf(stderr, "end.\n");
      break;
    }
    if(in_buf[0] == '!') {
      zero_all_values(&conv);
      continue;
    }
    if(in_buf[0] == '/') {
      // it's a comment line
      printf(in_buf);
      continue;
    }
    if(reading_name) {
      strcpy(name, in_buf);
      reading_name = false;
    } else {
      reading_name = true;
      double value = strtod(in_buf, &strtod_ptr);
      handleCommand(name, value, &conv);
    }
  }
}
