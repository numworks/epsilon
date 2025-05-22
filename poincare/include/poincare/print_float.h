#ifndef POINCARE_PRINT_FLOAT_H
#define POINCARE_PRINT_FLOAT_H

#include <assert.h>
#include <poincare/preferences.h>
#include <stdint.h>

namespace Poincare {

/*
 * This class holds static functions to represent float (as strings).
 */
class PrintFloat {
 public:
  constexpr static size_t k_floatNumberOfSignificantDigits = 7;
  constexpr static size_t k_maxNumberOfSignificantDigits = 14;
  // ᴇ is 3 bytes long
  constexpr static size_t k_specialECodePointByteLength = 3;
  /* We here define the glyph length and the buffer size to write the longest
   * float possible.
   * At maximum, the number has 15 significant digits so, in the worst case it
   * has the form -1.99999999999999ᴇ-308 (2+15+3+1+3 char) (the decimal mode is
   * always shorter. */
  constexpr static size_t k_maxFloatGlyphLength =
      2                                 // '-' and '.'
      + k_maxNumberOfSignificantDigits  // mantissa
      + 1                               // ᴇ
      + 1                               // exponant '-'
      + 3;                              // exponant
  constexpr static size_t k_maxFloatCharSize =
      2                                 // '-' and '.'
      + k_maxNumberOfSignificantDigits  // mantissa
      + k_specialECodePointByteLength   // ᴇ
      + 1                               // exponant '-'
      + 3                               // exponant
      + 1;                              // null-terminated

  constexpr static size_t glyphLengthForFloatWithPrecision(
      size_t numberOfSignificantDigits) {
    // The worst case is -1.234ᴇ-328
    return 2                            // '-' and '.'
           + numberOfSignificantDigits  // mantissa
           + 1                          // glyph ᴇ
           + 1                          // '-'
           + 3;                         // exponant
  }
  constexpr static size_t charSizeForFloatsWithPrecision(
      size_t numberOfSignificantDigits) {
    // The worst case is -1.234ᴇ-328
    return 2                                // '-' and '.'
           + numberOfSignificantDigits      // mantissa
           + k_specialECodePointByteLength  // ᴇ
           + 1                              // exponant '-'
           + 3                              // exponant
           + 1;                             // null-terminated
  }
  /* Since various apps limit the number of digits in exact results, there's no
   * need to lay out or serialize too many digits in decimals. */
  constexpr static size_t k_maxNumberOfSignificantDigitsInDecimals = 30;

  struct TextLengths {
    size_t CharLength;
    size_t GlyphLength;

    constexpr bool operator==(const TextLengths& other) const = default;
  };
  /* If the buffer size is too small to display the right number of significant
   * digits, the function forces the scientific mode. If the buffer is still too
   * small, the text representing the float is empty.
   * ConvertFloatToText returns the number of characters that have been written
   * in buffer (excluding the last \0 character). */
  template <class T>
  static TextLengths ConvertFloatToText(T d, char* buffer, size_t bufferSize,
                                        size_t availableGlyphLength,
                                        size_t numberOfSignificantDigits,
                                        Preferences::PrintFloatMode mode);
  template <class T>
  constexpr static size_t SignificantDecimalDigits() {
    return sizeof(T) == sizeof(double) ? k_maxNumberOfSignificantDigits
                                       : k_floatNumberOfSignificantDigits;
  }
  template <class T>
  constexpr static T DecimalModeMinimalValue() {
    return sizeof(T) == sizeof(double) ? 1e-3 : 1e-3f;
  }

  // Engineering notation
  static int EngineeringExponentFromBase10Exponent(int exponent);
  static size_t EngineeringMinimalNumberOfDigits(int exponentBase10,
                                                 int exponentEngineering) {
    int result = exponentBase10 - exponentEngineering + 1;
    assert(result > 0 && result <= 3);
    return static_cast<size_t>(result);
  }
  static size_t EngineeringNumberOfZeroesToAdd(
      size_t engineeringMinimalNumberOfDigits, size_t numberOfDigits) {
    int number = static_cast<int>(engineeringMinimalNumberOfDigits) -
                 static_cast<int>(numberOfDigits);
    return static_cast<size_t>(number > 0 ? number : 0);
  }

 private:
  template <class T>
  static TextLengths ConvertFloatToTextPrivate(
      T f, char* buffer, size_t bufferSize, size_t availableGlyphLength,
      size_t numberOfSignificantDigits, Preferences::PrintFloatMode mode);

  class Long final {
   public:
    Long(int64_t i = 0);
    Long(uint32_t d1, uint32_t, bool negative);
    bool isNegative() const { return m_negative; }
    bool isZero() const { return (m_digits[0] == 0) && (m_digits[1] == 0); }
    static void DivisionByTen(const Long& longToDivide, Long* quotient,
                              Long* digit);
    static void MultiplySmallLongByTen(Long& smallLong);

    size_t serialize(char* buffer, size_t bufferSize) const;
    uint32_t digit(uint8_t i) const {
      assert(i >= 0 && i < k_numberOfDigits);
      return m_digits[i];
    }

   private:
    constexpr static int64_t k_base = 1000000000;
    constexpr static size_t k_numberOfDigits = 2;
    constexpr static size_t k_maxNumberOfCharsForDigit = 9;

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
  static void PrintLongWithDecimalMarker(char* buffer, size_t bufferLength,
                                         Long& i, int decimalMarkerPosition);
};

}  // namespace Poincare

#endif
