#include <poincare/print_int.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

bool PrintIntInBuffer(uint32_t integer, char * buffer, int bufferLength, bool left) {
  assert(integer >= 0);
  if (bufferLength == 0) {
    return false;
  }

  // Case integer is 0
  if (integer == 0) {
    if (left) {
      buffer[0] = '0';
    } else {
      for (int i = 0; i < bufferLength; i++) {
        buffer[i] = '0';
      }
    }
    return true;
  }

  int wantedLength = std::log10(integer*1.0)+1;
  if (wantedLength > bufferLength) {
    return false;
  }
  int modulo = 10;
  int startIndex = left ? wantedLength - 1 : bufferLength - 1;
  for (int i = startIndex; i >= 0; i--) {
    int c = integer % modulo;
    buffer[i] = '0' + c;
    integer = (integer - c) / modulo;
    /* If the integer is written on the right, after all digits are written
     * integer is equal to 0 and the buffer is padded with 0 on the left. */
  }
  return true;
}


bool PrintInt::Left(uint32_t integer, char * buffer, int bufferLength) {
  return PrintIntInBuffer(integer, buffer, bufferLength, true);
}

bool PrintInt::Right(uint32_t integer, char * buffer, int bufferLength) {
  return PrintIntInBuffer(integer, buffer, bufferLength, false);
}

}
