#include <poincare/print_float.h>
#include <poincare/ieee754.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/preferences.h>
#include <poincare/undefined.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>
}
#include <cmath>

namespace Poincare {

void PrintFloat::printBase10IntegerWithDecimalMarker(char * buffer, int bufferLength, Integer i, int decimalMarkerPosition) {
  /* The decimal marker position is always preceded by a char, thus, it is never
   * in first position. When called by convertFloatToText, the buffer length is
   * always > 0 as we asserted a minimal number of available chars. */
  assert(bufferLength > 0 && decimalMarkerPosition != 0);
  char tempBuffer[PrintFloat::k_maxFloatBufferLength];
  int intLength = i.serialize(tempBuffer, PrintFloat::k_maxFloatBufferLength);
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

template <class T>
int PrintFloat::convertFloatToText(T f, char * buffer, int bufferSize,
    int numberOfSignificantDigits, Preferences::PrintFloatMode mode) {
  assert(numberOfSignificantDigits > 0);
  char tempBuffer[PrintFloat::k_maxFloatBufferLength];
  int requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits, mode);
  /* if the required buffer size overflows the buffer size, we first force the
   * display mode to scientific and decrease the number of significant digits to
   * fit the buffer size. If the buffer size is still to small, we only write
   * the beginning of the float and truncate it (which can result in a non sense
   * text) */
  if (mode == Preferences::PrintFloatMode::Decimal && requiredLength >= bufferSize) {
    requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits, Preferences::PrintFloatMode::Scientific);
  }
  if (requiredLength >= bufferSize) {
    int adjustedNumberOfSignificantDigits = numberOfSignificantDigits - requiredLength + bufferSize - 1;
    adjustedNumberOfSignificantDigits = adjustedNumberOfSignificantDigits < 1 ? 1 : adjustedNumberOfSignificantDigits;
    requiredLength = convertFloatToTextPrivate(f, tempBuffer, adjustedNumberOfSignificantDigits, Preferences::PrintFloatMode::Scientific);
  }
  requiredLength = requiredLength < bufferSize ? requiredLength : bufferSize-1;
  strlcpy(buffer, tempBuffer, bufferSize);
  return requiredLength;
}

template <class T>
int PrintFloat::convertFloatToTextPrivate(T f, char * buffer, int numberOfSignificantDigits, Preferences::PrintFloatMode mode) {
  assert(numberOfSignificantDigits > 0);
  if (std::isinf(f)) {
    assert(Infinity::NameSize()+1 < PrintFloat::k_maxFloatBufferLength);
    int currentChar = 0;
    if (f < 0) {
      buffer[currentChar++] = '-';
    }
    strlcpy(&buffer[currentChar], Infinity::Name(), PrintFloat::k_maxFloatBufferLength-1);
    return currentChar + Infinity::NameSize() - 1;
  }

  if (std::isnan(f)) {
    assert(Undefined::NameSize() < PrintFloat::k_maxFloatBufferLength);
    strlcpy(buffer, Undefined::Name(), PrintFloat::k_maxFloatBufferLength);
    return Undefined::NameSize() - 1;
  }

  int exponentInBase10 = IEEE754<T>::exponentBase10(f);

  /* Part I: Mantissa */

  // Compute mantissa
  T unroundedMantissa = f * std::pow((T)10.0, (T)(numberOfSignificantDigits - 1 - exponentInBase10));
  // Round mantissa to get the right number of significant digits
  T mantissa = std::round(unroundedMantissa);

  /* if numberOfSignificantDigits -1 - exponentInBase10
   * is too big (or too small), mantissa is now inf. We handle this case by
   * using logarithm function. */
  if (std::isnan(mantissa) || std::isinf(mantissa)) {
    mantissa = std::round(std::pow(10, std::log10(std::fabs(f))+(T)(numberOfSignificantDigits -1 - exponentInBase10)));
    mantissa = std::copysign(mantissa, f);
  }
  /* We update the exponent in base 10 (if 0.99999999 was rounded to 1 for
   * instance)
   * NB: the following if-condition would rather be:
   * "exponentBase10(unroundedMantissa) != exponentBase10(mantissa)",
   * however, unroundedMantissa can have a different exponent than expected
   * (ex: f = 1E13, unroundedMantissa = 99999999.99 and mantissa = 1000000000) */
  if (f != 0 && IEEE754<T>::exponentBase10(mantissa)-exponentInBase10 != numberOfSignificantDigits - 1 - exponentInBase10) {
    exponentInBase10++;
  }

  if (mode == Preferences::PrintFloatMode::Decimal && exponentInBase10 >= numberOfSignificantDigits) {
    /* Exception 1: avoid inventing digits to fill the printed float: when
     * displaying 12345 with 2 significant digis in Decimal mode for instance.
     * This exception is caught by convertFloatToText and forces the mode to
     * Scientific */
    return INT_MAX;
  }

  // Correct the number of digits in mantissa after rounding
  if (IEEE754<T>::exponentBase10(mantissa) >= numberOfSignificantDigits) {
    mantissa = mantissa/10.0;
  }

  // Number of chars for the mantissa
  int numberOfCharsForMantissaWithoutSign = exponentInBase10 >= 0 || mode == Preferences::PrintFloatMode::Scientific ? numberOfSignificantDigits : numberOfSignificantDigits - exponentInBase10;

  /* The number of digits in an mantissa is capped because the maximal int64_t
   * is 2^63 - 1. As our mantissa is an integer built from an int64_t, we assert
   * that we stay beyond this threshold during computation. */
  assert(numberOfSignificantDigits < std::log10(std::pow(2.0f, 63.0f)));

  // Supress the 0 on the right side of the mantissa
  Integer dividend = Integer((int64_t)mantissa);
  Integer quotient = Integer::Division(dividend, Integer(10)).quotient;
  Integer digit = Integer::Subtraction(dividend, Integer::Multiplication(quotient, Integer(10)));
  int minimumNumberOfCharsInMantissa = 1;
  while (digit.isZero() && numberOfCharsForMantissaWithoutSign > minimumNumberOfCharsInMantissa &&
      (numberOfCharsForMantissaWithoutSign > exponentInBase10+1 || mode == Preferences::PrintFloatMode::Scientific)) {
    numberOfCharsForMantissaWithoutSign--;
    dividend = quotient;
    quotient = Integer::Division(dividend, Integer(10)).quotient;
    digit = Integer::Subtraction(dividend, Integer::Multiplication(quotient, Integer(10)));
  }

  /* Part II: Decimal marker */

  // Force a decimal marker if there is fractional part
  bool decimalMarker = (mode == Preferences::PrintFloatMode::Scientific && numberOfCharsForMantissaWithoutSign > 1) || (mode == Preferences::PrintFloatMode::Decimal && numberOfCharsForMantissaWithoutSign > exponentInBase10 +1);
  if (decimalMarker) {
    numberOfCharsForMantissaWithoutSign++;
  }

  /* Find the position of the decimal marker position */
  int decimalMarkerPosition = exponentInBase10 < 0 || mode == Preferences::PrintFloatMode::Scientific ? 1 : exponentInBase10+1;
  decimalMarkerPosition = f < 0 ? decimalMarkerPosition+1 : decimalMarkerPosition;

  /* Part III: Exponent */

  int numberOfCharExponent = exponentInBase10 != 0 ? std::log10(std::fabs((T)exponentInBase10)) + 1 : 1;
  if (exponentInBase10 < 0){
    // If the exponent is < 0, we need a additional char for the sign
    numberOfCharExponent++;
  }

  /* Part III: print mantissa*10^exponent*/
  int numberOfCharsForMantissaWithSign = f >= 0 ? numberOfCharsForMantissaWithoutSign : numberOfCharsForMantissaWithoutSign + 1;
  // Print mantissa
  assert(!dividend.isInfinity());
  if (numberOfCharsForMantissaWithSign >= PrintFloat::k_maxFloatBufferLength) {
    /* Exception 3: if we are about to overflow the buffer, we escape by
     * returning a big int. This will be caught by 'convertFloatToText' which
     * will force displayMode to Scientific. */
    assert(mode == Preferences::PrintFloatMode::Decimal);
    return INT_MAX;
  }
  assert(numberOfCharsForMantissaWithSign < PrintFloat::k_maxFloatBufferLength);
  PrintFloat::printBase10IntegerWithDecimalMarker(buffer, numberOfCharsForMantissaWithSign, dividend, decimalMarkerPosition);
  if (mode == Preferences::PrintFloatMode::Decimal || exponentInBase10 == 0) {
    buffer[numberOfCharsForMantissaWithSign] = 0;
    return numberOfCharsForMantissaWithSign;
  }
  // Print exponent
  assert(numberOfCharsForMantissaWithSign < PrintFloat::k_maxFloatBufferLength);
  buffer[numberOfCharsForMantissaWithSign] = Ion::Charset::Exponent;
  assert(numberOfCharExponent+numberOfCharsForMantissaWithSign+1 < PrintFloat::k_maxFloatBufferLength);
  PrintFloat::printBase10IntegerWithDecimalMarker(buffer+numberOfCharsForMantissaWithSign+1, numberOfCharExponent, Integer(exponentInBase10), -1);
  buffer[numberOfCharsForMantissaWithSign+1+numberOfCharExponent] = 0;
  return (numberOfCharsForMantissaWithSign+1+numberOfCharExponent);
}

template int PrintFloat::convertFloatToText<float>(float, char*, int, int, Preferences::Preferences::PrintFloatMode);
template int PrintFloat::convertFloatToText<double>(double, char*, int, int, Preferences::Preferences::PrintFloatMode);

}

