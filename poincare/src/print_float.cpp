#include <ion/unicode/utf8_decoder.h>
#include <ion/unicode/utf8_helper.h>
#include <poincare/decimal.h>
#include <poincare/ieee754.h>
#include <poincare/infinity.h>
#include <poincare/preferences.h>
#include <poincare/print_float.h>
#include <poincare/print_int.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
extern "C" {
#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
}
#include <algorithm>
#include <cmath>

namespace Poincare {

constexpr int PrintFloat::Long::k_maxNumberOfCharsForDigit;

PrintFloat::Long::Long(int64_t i) : m_negative(i < 0) {
  int64_t nonNegativeI = m_negative ? -i : i;
  m_digits[1] = 0 + (nonNegativeI % k_base);
  m_digits[0] = (nonNegativeI - m_digits[1]) / k_base;
}

PrintFloat::Long::Long(uint32_t d1, uint32_t d2, bool negative) {
  m_digits[1] = d2;
  m_digits[0] = d1;
  m_negative = negative && !isZero();
}

void PrintFloat::Long::DivisionByTen(const Long& longToDivide, Long* quotient,
                                     Long* digit) {
  assert(quotient != nullptr && digit != nullptr);
  if (longToDivide.isZero()) {
    *quotient = Long(0);
    *digit = Long(0);
    return;
  }

  uint32_t digit0 = longToDivide.digit(0);
  uint32_t digit1 = longToDivide.digit(1);

  uint32_t d = digit1 % 10;
  uint32_t digit1DividedByTen = (digit1 - d) / 10;
  *digit = Long(d);

  if (digit0 == 0) {
    *quotient = Long(0, digit1DividedByTen, longToDivide.isNegative());
  } else {
    *quotient =
        Long(digit0 / 10, digit1DividedByTen + (k_base / 10) * (digit0 % 10),
             longToDivide.isNegative());
  }
}

void PrintFloat::Long::MultiplySmallLongByTen(Long& smallLong) {
  assert(smallLong.digit(0) == 0 && smallLong.digit(1) < (k_base / 1000) &&
         !smallLong.isZero());
  uint32_t newDigit1 = smallLong.digit(1) * 10;
  smallLong = Long(0, newDigit1, smallLong.isNegative());
}

int PrintFloat::Long::serialize(char* buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return bufferSize - 1;
  }
  if (bufferSize == 1) {
    buffer[0] = 0;
    return bufferSize - 1;
  }
  int numberOfChars = m_negative ? 1 : 0;  // 1 for the minus sign char
  if (m_digits[0] != 0) {
    numberOfChars += PrintInt::Left(m_digits[0], buffer + numberOfChars,
                                    bufferSize - numberOfChars - 1);
    int wantedNumberOfChars = numberOfChars + k_maxNumberOfCharsForDigit + 1;
    if (wantedNumberOfChars > bufferSize) {
      /* There is not enough space for serializing the second digit and the null
       * terminating char. */
      return wantedNumberOfChars;
    }
    numberOfChars += PrintInt::Right(
        m_digits[1], buffer + numberOfChars,
        std::min(k_maxNumberOfCharsForDigit, bufferSize - numberOfChars - 1));
  } else {
    numberOfChars += PrintInt::Left(m_digits[1], buffer + numberOfChars,
                                    bufferSize - numberOfChars - 1);
  }
  if (m_negative) {
    assert(UTF8Decoder::CharSizeOfCodePoint('-') == 1);
    if (bufferSize > 0) {
      buffer[0] = '-';
    }
  }
  if (numberOfChars <= bufferSize - 1) {
    buffer[numberOfChars] = 0;
  }
  return numberOfChars;
}

void PrintFloat::PrintLongWithDecimalMarker(char* buffer, int bufferLength,
                                            Long& i,
                                            int decimalMarkerPosition) {
  /* The decimal marker position is always preceded by a char, thus, it is never
   * in first position. When called by ConvertFloatToText, the buffer length is
   * always > 0 as we asserted a minimal number of available chars. */
  assert(bufferLength > 0 && decimalMarkerPosition != 0);
  constexpr int tempBufferSize = PrintFloat::k_maxFloatCharSize;
  char tempBuffer[tempBufferSize];
  int intLength = i.serialize(tempBuffer, tempBufferSize);
  int firstDigitChar = UTF8Helper::CodePointIs(tempBuffer, '-') ? 1 : 0;
  /* We should use the UTF8Decoder to write code points in buffers, but it is
   * much clearer to manipulate chars directly as we know that the code point we
   * use ('.', '0, '1', '2', ...) are only one char long. */
  for (int k = bufferLength - 1; k >= firstDigitChar; k--) {
    if (k == decimalMarkerPosition) {
      assert(UTF8Decoder::CharSizeOfCodePoint('.') == 1);
      buffer[k] = '.';
      continue;
    }
    if (intLength > firstDigitChar) {
      assert(UTF8Decoder::CharSizeOfCodePoint(tempBuffer[intLength - 1]) == 1);
      buffer[k] = tempBuffer[--intLength];
      continue;
    }
    assert(UTF8Decoder::CharSizeOfCodePoint('0') == 1);
    buffer[k] = '0';
  }
  if (firstDigitChar == 1) {
    assert(UTF8Decoder::CharSizeOfCodePoint(tempBuffer[0]) == 1);
    buffer[0] = tempBuffer[0];
  }
}

template <class T>
PrintFloat::TextLengths PrintFloat::ConvertFloatToText(
    T f, char* buffer, int bufferSize, int glyphLength,
    int numberOfSignificantDigits, Preferences::PrintFloatMode mode) {
  assert(numberOfSignificantDigits > 0);
  assert(bufferSize > 0);

  /* Assert that the glyphLength is capped.
   * Example: 1+1.234E-30+... in decimal mode, because we do not want the fill
   * the buffer with the decimal version of 1.234E-30. */
  assert(glyphLength <= k_maxFloatGlyphLength);

  bool forceScientific = mode == Preferences::PrintFloatMode::Decimal &&
                         std::fabs(f) < DecimalModeMinimalValue<T>();
  TextLengths requiredLengths;
  if (!forceScientific) {
    requiredLengths = ConvertFloatToTextPrivate(
        f, buffer, bufferSize, glyphLength, numberOfSignificantDigits, mode);
    forceScientific = (mode == Preferences::PrintFloatMode::Decimal &&
                       (requiredLengths.CharLength > bufferSize - 1 ||
                        requiredLengths.GlyphLength > glyphLength));
  }
  if (forceScientific) {
    /* If the required buffer size overflows the buffer size, we force the
     * display mode to scientific. We also force it if the number is too small,
     * to avoid displaying things like 0.0000001. */
    requiredLengths = ConvertFloatToTextPrivate(
        f, buffer, bufferSize, glyphLength, numberOfSignificantDigits,
        Preferences::PrintFloatMode::Scientific);
  }

  if (requiredLengths.CharLength > bufferSize - 1 ||
      requiredLengths.GlyphLength > glyphLength) {
    buffer[0] = 0;
    /* We still return the required sizes even if we could not write the float
     * in the buffer in order to indicate that we overflew the buffer. */
  }
  return requiredLengths;
}

int PrintFloat::EngineeringExponentFromBase10Exponent(int exponent) {
  int exponentMod3 = exponent % 3;
  int exponentInEngineeringNotation =
      exponent - (exponentMod3 + (exponentMod3 < 0 ? 3 : 0));
  assert(exponentInEngineeringNotation % 3 == 0);
  assert(exponentInEngineeringNotation <= exponent &&
         exponent < exponentInEngineeringNotation + 3);
  return exponentInEngineeringNotation;
}

template <class T>
PrintFloat::TextLengths PrintFloat::ConvertFloatToTextPrivate(
    T f, char* buffer, int bufferSize, int glyphLength,
    int numberOfSignificantDigits, Preferences::PrintFloatMode mode) {
  assert(numberOfSignificantDigits > 0);
  assert(bufferSize > 0);
  assert(glyphLength > 0 && glyphLength <= k_maxFloatGlyphLength);
  int availableCharLength = std::min(bufferSize - 1, glyphLength);
  TextLengths exceptionResult = {.CharLength = bufferSize,
                                 .GlyphLength = glyphLength + 1};
  // TODO: accelerate for f between 0 and 10 ?
  if (std::isinf(f)) {
    // Infinity
    int requiredCharLength = Infinity::NameSize(f < 0) - 1;
    TextLengths requiredTextLengths = {.CharLength = requiredCharLength,
                                       .GlyphLength = requiredCharLength};
    if (requiredCharLength > availableCharLength) {
      // We will not be able to print
      return requiredTextLengths;
    }
    // Write inf or -inf
    strlcpy(buffer, Infinity::Name(f < 0), bufferSize);
    return requiredTextLengths;
  }

  if (std::isnan(f)) {
    // Nan
    constexpr int requiredCharLength = Undefined::NameSize() - 1;
    constexpr TextLengths requiredTextLengths = {
        .CharLength = requiredCharLength, .GlyphLength = requiredCharLength};
    if (requiredCharLength > availableCharLength) {
      // We will not be able to print
      return requiredTextLengths;
    }
    strlcpy(buffer, Undefined::Name(), bufferSize);
    return requiredTextLengths;
  }

  int exponentInBase10 = IEEE754<T>::exponentBase10(f);

  /* Part I: Mantissa */

  /* Compute mantissa
   * We compute the unroundedMantissa using doubles to limit approximation
   * errors. Previously when computing with floats : 0.000600000028 * 10^10 =
   * 6000000.28 was rounded into 6000000.5 because of the conversion error
   * Mantissa was the 6000001 instead of 6000000.
   * As a result, 0.0006 was displayed as 0.0006000001
   * With doubles, 0.000600000028 * 10^10 = 6000000.2849...
   * This value is then rounded into mantissa = 6000000 which yields a proper
   * display of 0.0006 */
  double unroundedMantissa = static_cast<double>(f) *
                             std::pow(10.0, (double)(numberOfSignificantDigits -
                                                     1 - exponentInBase10));
  // Round mantissa to get the right number of significant digits
  double mantissa = std::round(unroundedMantissa);

  /* Since no problem of approximation was detected from using potential float
   * (instead of double) in the rest of the code, we decided to leave it like
   * this */

  /* If (numberOfSignificantDigits - 1 - exponentInBase10) is too big (or too
   * small), mantissa is now inf. We handle this case by using logarithm
   * function. */
  if (std::isnan(mantissa) || std::isinf(mantissa)) {
    mantissa =
        std::round(std::pow(10, std::log10(std::fabs(f)) +
                                    static_cast<T>(numberOfSignificantDigits -
                                                   1 - exponentInBase10)));
    mantissa = std::copysign(mantissa, static_cast<double>(f));
  }
  /* We update the exponent in base 10 (if 0.99999999 was rounded to 1 for
   * instance)
   * NB: the following if-condition should rather be:
   * "exponentBase10(unroundedMantissa) != exponentBase10(mantissa)",
   * However, unroundedMantissa can have a different exponent than expected
   * (ex: f = 1E13, unroundedMantissa = 99999999.99 and mantissa = 1000000000)
   */
  if (f != 0 && IEEE754<double>::exponentBase10(mantissa) - exponentInBase10 !=
                    numberOfSignificantDigits - 1 - exponentInBase10) {
    exponentInBase10++;
  }

  if (mode == Preferences::PrintFloatMode::Decimal &&
      exponentInBase10 >= numberOfSignificantDigits) {
    /* Exception 1: avoid inventing digits to fill the printed float: when
     * displaying 12345 with 2 significant digis in Decimal mode for instance.
     * This exception is caught by ConvertFloatToText and forces the mode to
     * Scientific */
    return exceptionResult;
  }

  // Correct the number of digits in mantissa after rounding
  if (IEEE754<T>::exponentBase10(mantissa) >= numberOfSignificantDigits) {
    mantissa = mantissa / static_cast<T>(10.0);
  }

  // Number of chars for the mantissa
  int numberOfCharsForMantissaWithoutSign = 0;
  if (mode == Preferences::PrintFloatMode::Decimal) {
    if (exponentInBase10 >= 0) {
      numberOfCharsForMantissaWithoutSign = numberOfSignificantDigits;
    } else {
      /* exponentInBase10 < 0, so we add |exponentInBase10| to count 0 added
       * before significant digits */
      numberOfCharsForMantissaWithoutSign =
          numberOfSignificantDigits - exponentInBase10;
    }
  } else if (mode == Preferences::PrintFloatMode::Scientific) {
    numberOfCharsForMantissaWithoutSign = numberOfSignificantDigits;
  } else {
    assert(mode == Preferences::PrintFloatMode::Engineering);
    numberOfCharsForMantissaWithoutSign = numberOfSignificantDigits;
  }

  /* The number of digits in a mantissa is capped because the maximal int64_t is
   * 2^63 - 1. As our mantissa is an integer built from an int64_t, we assert
   * that we stay beyond this threshold during computation. */
  assert(numberOfSignificantDigits < std::log10(std::pow(2.0f, 63.0f)));

  // Remove/Add the zeroes on the right side of the mantissa
  Long dividend = Long((int64_t)mantissa);

  int exponentForEngineeringNotation = 0;
  int minimalNumberOfMantissaDigits = 1;
  bool removeZeroes = true;

  if (mode == Preferences::PrintFloatMode::Engineering) {
    exponentForEngineeringNotation =
        EngineeringExponentFromBase10Exponent(exponentInBase10);
    minimalNumberOfMantissaDigits = EngineeringMinimalNumberOfDigits(
        exponentInBase10, exponentForEngineeringNotation);
    int numberOfZeroesToAdd = EngineeringNumberOfZeroesToAdd(
        minimalNumberOfMantissaDigits, numberOfCharsForMantissaWithoutSign);
    if (numberOfZeroesToAdd > 0) {
      removeZeroes = false;
      assert(numberOfCharsForMantissaWithoutSign - numberOfSignificantDigits <
             3);
      for (int i = 0; i < numberOfZeroesToAdd; i++) {
        assert(mantissa < 1000);
        Long::MultiplySmallLongByTen(dividend);
      }
    }
  }
  if (removeZeroes) {
    Long digit;
    Long quotient;
    Long::DivisionByTen(dividend, &quotient, &digit);
    int minimumNumberOfCharsInMantissa =
        mode == Preferences::PrintFloatMode::Engineering
            ? minimalNumberOfMantissaDigits
            : 1;
    int numberOfZerosRemoved = 0;
    while (digit.isZero() &&
           numberOfCharsForMantissaWithoutSign >
               minimumNumberOfCharsInMantissa &&
           (numberOfCharsForMantissaWithoutSign > exponentInBase10 + 1 ||
            mode == Preferences::PrintFloatMode::Scientific ||
            mode == Preferences::PrintFloatMode::Engineering)) {
      assert(UTF8Decoder::CharSizeOfCodePoint('0') == 1);
      numberOfCharsForMantissaWithoutSign--;
      dividend = quotient;
      Long::DivisionByTen(dividend, &quotient, &digit);
      numberOfZerosRemoved++;
    }
    if (numberOfCharsForMantissaWithoutSign > availableCharLength) {
      // Escape now if the true number of needed digits is not required
      return exceptionResult;
    }
  }

  /* Part II: Decimal marker */

  // Force a decimal marker if there is fractional part
  bool decimalMarker =
      (mode == Preferences::PrintFloatMode::Scientific &&
       numberOfCharsForMantissaWithoutSign > 1) ||
      (mode == Preferences::PrintFloatMode::Decimal &&
       numberOfCharsForMantissaWithoutSign > exponentInBase10 + 1) ||
      (mode == Preferences::PrintFloatMode::Engineering &&
       (numberOfCharsForMantissaWithoutSign > minimalNumberOfMantissaDigits));
  if (decimalMarker) {
    assert(UTF8Decoder::CharSizeOfCodePoint('.') == 1);
    numberOfCharsForMantissaWithoutSign++;
  }
  if (numberOfCharsForMantissaWithoutSign > availableCharLength) {
    // Escape now if the true number of needed digits is not required
    return exceptionResult;
  }

  /* Find the position of the decimal marker position */
  int decimalMarkerPosition = 0;
  if (mode == Preferences::PrintFloatMode::Decimal) {
    decimalMarkerPosition = (exponentInBase10 < 0) ? 1 : exponentInBase10 + 1;
  } else if (mode == Preferences::PrintFloatMode::Scientific) {
    decimalMarkerPosition = 1;
  } else {
    assert(mode == Preferences::PrintFloatMode::Engineering);
    decimalMarkerPosition = minimalNumberOfMantissaDigits;
  }
  if (f < 0) {
    decimalMarkerPosition++;
  }
  assert(UTF8Decoder::CharSizeOfCodePoint('-') == 1);

  /* Part III: Sign */

  int numberOfCharsForMantissaWithSign =
      numberOfCharsForMantissaWithoutSign + (f >= 0 ? 0 : 1);
  if (numberOfCharsForMantissaWithSign > availableCharLength) {
    // Exception 2: we will overflow the buffer
    return exceptionResult;
  }

  /* Part IV: Exponent */

  int exponent = mode == Preferences::PrintFloatMode::Engineering
                     ? exponentForEngineeringNotation
                     : exponentInBase10;
  int numberOfCharExponent =
      exponent != 0 ? IEEE754<T>::exponentBase10(static_cast<T>(exponent)) + 1
                    : 0;
  if (exponent < 0) {
    // If the exponent is < 0, we need a additional char for the sign
    numberOfCharExponent++;
    assert(UTF8Decoder::CharSizeOfCodePoint('-') == 1);
  }

  /* Part V: print mantissa*10^exponent */

  assert(UTF8Decoder::CharSizeOfCodePoint('-') == 1);
  // Print mantissa
  bool doNotWriteExponent =
      (mode == Preferences::PrintFloatMode::Decimal) || (exponent == 0);
  int neededNumberOfChars = numberOfCharsForMantissaWithSign;
  int neededNumberOfGlyphs = numberOfCharsForMantissaWithSign;
  if (!doNotWriteExponent) {
    neededNumberOfChars +=
        UTF8Decoder::CharSizeOfCodePoint(UCodePointLatinLetterSmallCapitalE) +
        numberOfCharExponent;
    neededNumberOfGlyphs += 1 + numberOfCharExponent;
  }
  if (neededNumberOfChars > bufferSize - 1 ||
      neededNumberOfGlyphs > glyphLength) {
    // Exception 3: We are about to overflow the buffer.
    return exceptionResult;
  }
  PrintLongWithDecimalMarker(buffer, numberOfCharsForMantissaWithSign, dividend,
                             decimalMarkerPosition);
  if (doNotWriteExponent) {
    buffer[numberOfCharsForMantissaWithSign] = 0;
    return {.CharLength = numberOfCharsForMantissaWithSign,
            .GlyphLength = numberOfCharsForMantissaWithSign};
  }
  // Print exponent
  assert(numberOfCharsForMantissaWithSign < bufferSize);
  int currentNumberOfChar = numberOfCharsForMantissaWithSign;
  currentNumberOfChar += UTF8Decoder::CodePointToChars(
      UCodePointLatinLetterSmallCapitalE, buffer + currentNumberOfChar,
      bufferSize - currentNumberOfChar - 1);
  dividend = Long(exponent);  // reuse dividend as it is not needed anymore
  PrintLongWithDecimalMarker(buffer + currentNumberOfChar, numberOfCharExponent,
                             dividend, -1);
  buffer[currentNumberOfChar + numberOfCharExponent] = 0;
  assert(neededNumberOfChars == currentNumberOfChar + numberOfCharExponent);
  return {.CharLength = currentNumberOfChar + numberOfCharExponent,
          .GlyphLength =
              numberOfCharsForMantissaWithSign + 1 + numberOfCharExponent};
}

template PrintFloat::TextLengths PrintFloat::ConvertFloatToText<float>(
    float, char*, int, int, int, Preferences::Preferences::PrintFloatMode);
template PrintFloat::TextLengths PrintFloat::ConvertFloatToText<double>(
    double, char*, int, int, int, Preferences::Preferences::PrintFloatMode);
template int PrintFloat::SignificantDecimalDigits<float>();
template int PrintFloat::SignificantDecimalDigits<double>();

}  // namespace Poincare
