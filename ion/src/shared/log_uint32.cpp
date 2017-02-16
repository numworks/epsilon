#include <ion/log.h>

#ifdef DEBUG

void ion_log_uint32(uint32_t i) {
  char buffer[9];
  for (int j=0; j<8; j++) {
    uint8_t v = (i & 0xF);
    buffer[8-1-j] = (v>9) ? 'A'+v-10 : '0'+v;
    i = i >> 4;
  }
  buffer[8] = 0;
  ion_log_string(buffer);
}

#endif
