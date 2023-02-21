#include <assert.h>
#include <ion.h>
#include <quiz.h>

QUIZ_CASE(ion_crc32) {
  uint32_t inputWord[] = {0x48656C6C, 0x6F2C2077};
  quiz_assert(Ion::crc32Word(inputWord, 1) == 0x93591FFD);
  quiz_assert(Ion::crc32Word(inputWord, 2) == 0x72EAD3FB);

  uint8_t inputBytes[] = {0x6C, 0x6C, 0x65, 0x48, 0x77, 0x20, 0x2C, 0x6F};
  quiz_assert(Ion::crc32Byte(inputBytes, 1) == 0xD7A1E247);
  quiz_assert(Ion::crc32Byte(inputBytes, 4) == 0x93591FFD);
  quiz_assert(Ion::crc32Byte(inputBytes, 6) == 0x7BCD4EB3);
  quiz_assert(Ion::crc32Byte(inputBytes, 8) == 0x72EAD3FB);
}
