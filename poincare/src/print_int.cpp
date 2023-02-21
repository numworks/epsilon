#include <assert.h>
#include <poincare/ieee754.h>
#include <poincare/print_int.h>

#include <cmath>

namespace Poincare {

int PrintIntInBuffer(uint32_t integer, char* buffer, int bufferLength,
                     bool left) {
  int wantedLength = IEEE754<double>::exponentBase10((double)integer) + 1;
  assert(wantedLength > 0);
  if (bufferLength == 0 || wantedLength > bufferLength) {
    return wantedLength;
  }

  uint32_t modulo = 10;
  uint32_t startIndexPlusOne = left ? wantedLength : bufferLength;
  for (int i = startIndexPlusOne; i > 0; i--) {
    /* If the integer is written on the right, after all digits are written
     * integer is equal to 0 and the buffer is padded with 0 on the left. */
    uint32_t c = integer % modulo;
    buffer[i - 1] = '0' + c;
    integer = (integer - c) / modulo;
  }
  return wantedLength;
}

int PrintInt::Left(uint32_t integer, char* buffer, int bufferLength) {
  return PrintIntInBuffer(integer, buffer, bufferLength, true);
}

int PrintInt::Right(uint32_t integer, char* buffer, int bufferLength) {
  PrintIntInBuffer(integer, buffer, bufferLength, false);
  return bufferLength;
}

}  // namespace Poincare
