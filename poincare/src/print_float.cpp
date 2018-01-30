#include <poincare/print_float.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
}
#include <cmath>
#include <ion.h>
#include <stdio.h>

namespace Poincare {

void PrintFloat::printBase10IntegerWithDecimalMarker(char * buffer, int bufferLength, Integer i, int decimalMarkerPosition) {
  /* The decimal marker position is always preceded by a char, thus, it is never
   * in first position. When called by convertFloatToText, the buffer length is
   * always > 0 as we asserted a minimal number of available chars. */
  assert(bufferLength > 0 && decimalMarkerPosition != 0);
  char tempBuffer[PrintFloat::k_maxFloatBufferLength];
  int intLength = i.writeTextInBuffer(tempBuffer, PrintFloat::k_maxFloatBufferLength);
  int firstDigitChar = tempBuffer[0] == '-' ? 1 : 0;
  for (int k = bufferLength-1; k >= firstDigitChar; k--) {
    if (k == decimalMarkerPosition) {
      buffer[k] = '.';
      continue;
    }
    if (intLength > firstDigitChar) {
      buffer[k] = tempBuffer[--intLength];
      continue;
    }
    buffer[k] = '0';
  }
  if (firstDigitChar == 1) {
    buffer[0] = tempBuffer[0];
  }
}

}

