#include <quiz.h>
#include <ion.h>
#include <assert.h>

QUIZ_CASE(ion_crc32) {
  uint32_t input[] = { 0x48656C6C, 0x6F2C2077 };
  assert(Ion::crc32(input, 1) == 0x93591FFD);
  assert(Ion::crc32(input, 2) == 0x72EAD3FB);
}

