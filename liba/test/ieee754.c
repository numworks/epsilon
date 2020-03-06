#include <quiz.h>
#include <assert.h>
#include <math.h>
#include <private/ieee754.h>

QUIZ_CASE(liba_ieee754) {
  quiz_assert(ieee754man32(123.456f) == 7793017);
  quiz_assert(ieee754exp32(123.456f) == 133);
  quiz_assert(ieee754man32(555.555f) == 713605);
  quiz_assert(ieee754exp32(555.555f) == 136);
  quiz_assert(ieee754man32(0.007f) == 6643778);
  quiz_assert(ieee754exp32(0.007f) == 119);
  quiz_assert(ieee754man64(123.456) == 0b1110110111010010111100011010100111111011111001110111);
  quiz_assert(ieee754exp64(123.456) == 0b10000000101);
  quiz_assert(ieee754man64(555.555) == 0b0001010111000111000010100011110101110000101000111101);
  quiz_assert(ieee754exp64(555.555) == 0b10000001000);
  quiz_assert(ieee754man64(0.007) == 0b1100101011000000100000110001001001101110100101111001);
  quiz_assert(ieee754exp64(0.007) == 0b01111110111);
  quiz_assert(isinf(INFINITY));
  quiz_assert(isinf((double)INFINITY));
}
