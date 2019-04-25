#include <poincare/print_int.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

bool PrintInt::PadIntInBuffer(uint32_t integer, char * buffer, int bufferLength) {
  assert(integer >= 0);
  int wantedLength = std::log10(integer*1.0)+1;
  if (wantedLength > bufferLength) {
    return false;
  }
  int modulo = 10;
  for (int i = bufferLength - 1; i >= 0; i--) {
    int c = integer % modulo;
    buffer[i] = '0' + c;
    integer = (integer - c) / modulo;
    /* After all digits are written on the right of the buffer, integer is equal
     * to 0 and the buffer is padded with 0 on the left. */
  }
  return true;
}

}
