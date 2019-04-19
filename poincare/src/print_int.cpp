#include <poincare/print_int.h>
#include <assert.h>
#include <cmath>

namespace Poincare {

bool PrintInt::PadIntInBuffer(int integer, char * buffer, int bufferLength) {
  assert(integer >= 0);
  int wantedLength = std::log10(integer*1.0);
  if (wantedLength > bufferLength) {
    return false;
  }
  for (int i = 0; i < bufferLength; i++) {
    buffer[i] = '0' + ((int)(integer/std::pow(10.0, bufferLength - 1.0 - i))) - ((int)(integer/std::pow(10.0, bufferLength-i)))*10;
  }
  return true;
}

}
