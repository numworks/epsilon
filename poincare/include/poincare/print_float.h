#ifndef POINCARE_PRINT_FLOAT_H
#define POINCARE_PRINT_FLOAT_H

#include <assert.h>
#include <poincare/preferences.h>
#include <stdint.h>

namespace Poincare {

class PrintFloat {
public:
  constexpr static int k_numberOfPrintedSignificantDigits = 7;
  constexpr static int k_numberOfStoredSignificantDigits = 14;
  // ᴇ and ℯ are 3 bytes long
  constexpr static int k_specialECodePointByteLength = 3;
  /* We here define the buffer size to write the longest float possible.
   * At maximum, the number has 15 significant digits so, in the worst case it
   * has the form -1.99999999999999ᴇ-308 (2+15+3+1+3 char) (the auto mode is
   * always shorter. */
  constexpr static int k_maxFloatBufferLength = 2+k_numberOfStoredSignificantDigits+k_specialECodePointByteLength+1+3+1;

  constexpr static int bufferSizeForFloatsWithPrecision(int numberOfSignificantDigits) {
    // The worst case is -1.234ᴇ-328
    return 2+numberOfSignificantDigits+k_specialECodePointByteLength+1+3+1;
  }

  /* If the buffer size is too small to display the right number of significant
   * digits, the function forces the scientific mode and caps the number of
   * significant digits to fit the buffer. If the buffer is too small to display
   * any float, the text representing the float is truncated at the end of the
   * buffer.
   * ConvertFloatToText returns the number of characters that have been written
   * in buffer (excluding the last \0 character). */
  template <class T>
  static int convertFloatToText(T d, char * buffer, int bufferSize, int numberOfSignificantDigits, Preferences::PrintFloatMode mode, bool allowRounding = true);

private:
  template <class T>
  static int ConvertFloatToTextPrivate(T f, char * buffer, int bufferSize, int numberOfSignificantDigits, Preferences::PrintFloatMode mode, int * numberOfRemovedZeros);

  class Long final {
  public:
    Long(int64_t i = 0);
    Long(uint32_t d1, uint32_t, bool negative);
    bool isNegative() const { return m_negative; }
    bool isZero() const { return (m_digits[0] == 0) && (m_digits[1] == 0); }
    static void DivisionByTen(const Long & longToDivide, Long * quotient, Long * digit);

    int serialize(char * buffer, int bufferSize) const;
    uint32_t digit(uint8_t i) const {
      assert(i >= 0 && i < k_numberOfDigits);
      return m_digits[i];
    }
  private:
    constexpr static int64_t k_base = 1000000000;
    constexpr static int k_numberOfDigits = 2;
    constexpr static int k_maxNumberOfCharsForDigit = 8;

    bool m_negative;
    uint32_t m_digits[k_numberOfDigits];
  };

  /* This function prints the long i in the buffer with a '.' at the position
   * specified by the decimalMarkerPosition.
   * It starts printing at the end of the buffer and prints from right to left.
   * The given long should be of the right length to be written in bufferLength
   * chars. If the long is too small, the buffer is padded on the left with '0'.
   * If the long is too big, the printing stops when no more empty chars are
   * available, without returning any warning.
   * Warning: the buffer is not null terminated but is ensured to hold
   * bufferLength chars. */
  static void PrintLongWithDecimalMarker(char * buffer, int bufferLength, Long & i, int decimalMarkerPosition);


};

}

#endif
