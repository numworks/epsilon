#include <kandinsky/unicode/utf8decoder.h>
#include <assert.h>

static inline int leading_ones(uint8_t value) {
  for (int i=0; i<8; i++) {
    if (!(value & 0x80)) {
      return i;
    }
    value = value << 1;
  }
  assert(false);
}

static inline uint8_t last_k_bits(uint8_t value, uint8_t bits) {
  return (value & ((1<<bits)-1));
}

CodePoint UTF8Decoder::nextCodePoint() {
  int leadingOnes = leading_ones(*m_string);
  uint32_t result = last_k_bits(*m_string++, 8-leadingOnes-1);
  for (int i=0; i<(leadingOnes-1); i++) {
    result <<= 6;
    result += (*m_string++ & 0x3F);
  }
  return CodePoint(result);
}
