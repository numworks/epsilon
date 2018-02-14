#ifndef POINCARE_PRINT_FLOAT_H
#define POINCARE_PRINT_FLOAT_H

#include <poincare/integer.h>
#include <assert.h>

namespace Poincare {

namespace PrintFloat {
  /* The 'Mode' refers to the way to display float 'scientific' or 'auto'. The
   * scientific mode returns float with style -1.2E2 whereas the auto mode
   * tries to return 'natural' float like (0.021) and switches to scientific
   * mode if the float is too small or too big regarding the number of
   * significant digits. */
  enum class Mode {
    Decimal = 0,
    Scientific = 1,
    Default = 2
  };
  constexpr static int bufferSizeForFloatsWithPrecision(int numberOfSignificantDigits) {
    // The wors case is -1.234E-38
    return numberOfSignificantDigits + 7;
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
   * has the form -1.99999999999999e-308 (15+7+1 char) (the auto mode is always
   * shorter. */
  constexpr static int k_maxFloatBufferLength = k_numberOfStoredSignificantDigits+7+1;
  /* We here define the buffer size to write the lengthest complex possible.
   * The worst case has the form
   * -1.99999999999999e-308*e^(-1.99999999999999e-308*i) (14+14+7+1 char) */
  constexpr static int k_maxComplexBufferLength = k_maxFloatBufferLength-1+k_maxFloatBufferLength-1+7+1;

  /* If the buffer size is too small to display the right number of significant
   * digits, the function forces the scientific mode and cap the number of
   * significant digits to fit the buffer. If the buffer is too small to
   * display any float, the text representing the float is truncated at the end
   * of the buffer.
   * ConvertFloatToText return the number of characters that have been written
   * in buffer (excluding the last \O character) */
  template <class T>
  int convertFloatToText(T d, char * buffer, int bufferSize, int numberOfSignificantDigits, Mode mode = Mode::Default);
  template <class T>
  static int convertFloatToTextPrivate(T f, char * buffer, int numberOfSignificantDigits, Mode mode);
}

}

#endif
