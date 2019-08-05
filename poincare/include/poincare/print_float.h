#ifndef POINCARE_PRINT_FLOAT_H
#define POINCARE_PRINT_FLOAT_H

#include <assert.h>
#include <poincare/preferences.h>

namespace Poincare {

class Integer;


namespace PrintFloat {
  // ᴇ and ℯ are 3 bytes long
  constexpr static int k_specialECodePointByteLength = 3;

  constexpr static int bufferSizeForFloatsWithPrecision(int numberOfSignificantDigits) {
    // The worst case is -1.234ᴇ-328
    return 2+numberOfSignificantDigits+k_specialECodePointByteLength+1+3+1;
  }

  /* This function prints the integer i in the buffer with a '.' at the position
   * specified by the decimalMarkerPosition. It starts printing at the end of the
   * buffer and print from right to left. The integer given should be of the right
   * length to be written in bufferLength chars. If the integer is to small, the
   * empty chars on the left side are completed with '0'. If the integer is too
   * big, the printing stops when no more empty chars are available without
   * returning any warning.
   * Warning: the buffer is not null terminated but is ensured to hold
   * bufferLength chars. */
  void printBase10IntegerWithDecimalMarker(char * buffer, int bufferLength, Integer i, int decimalMarkerPosition);

  constexpr static int k_numberOfPrintedSignificantDigits = 7;
  constexpr static int k_numberOfStoredSignificantDigits = 14;

  /* We here define the buffer size to write the lengthest float possible.
   * At maximum, the number has 15 significant digits so, in the worst case it
   * has the form -1.99999999999999ᴇ-308 (2+15+3+1+3 char) (the auto mode is
   * always shorter. */
  constexpr static int k_maxFloatBufferLength = 2+k_numberOfStoredSignificantDigits+k_specialECodePointByteLength+1+3+1;

  /* If the buffer size is too small to display the right number of significant
   * digits, the function forces the scientific mode and caps the number of
   * significant digits to fit the buffer. If the buffer is too small to display
   * any float, the text representing the float is truncated at the end of the
   * buffer.
   * ConvertFloatToText returns the number of characters that have been written
   * in buffer (excluding the last \0 character). */
  template <class T>
  int convertFloatToText(T d, char * buffer, int bufferSize, int numberOfSignificantDigits, Preferences::PrintFloatMode mode, bool allowRounding = true);
  template <class T>
  static int convertFloatToTextPrivate(T f, char * buffer, int bufferSize, int numberOfSignificantDigits, Preferences::PrintFloatMode mode, int * numberOfRemovedZeros);
}

}

#endif
