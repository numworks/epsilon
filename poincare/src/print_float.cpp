#include <poincare/print_float.h>
#include <poincare/preferences.h>
#include <poincare/ieee754.h>
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

template <class T>
int PrintFloat::convertFloatToText(T f, char * buffer, int bufferSize,
    int numberOfSignificantDigits, Mode mode) {
  assert(numberOfSignificantDigits > 0);
  if (mode == Mode::Default) {
    return convertFloatToText(f, buffer, bufferSize, numberOfSignificantDigits, Preferences::sharedPreferences()->displayMode());
  }
  char tempBuffer[PrintFloat::k_maxFloatBufferLength];
  int requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits, mode);
  /* if the required buffer size overflows the buffer size, we first force the
   * display mode to scientific and decrease the number of significant digits to
   * fit the buffer size. If the buffer size is still to small, we only write
   * the beginning of the float and truncate it (which can result in a non sense
   * text) */
  if (mode == Mode::Decimal && requiredLength >= bufferSize) {
    requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits, Mode::Scientific);
  }
  if (requiredLength >= bufferSize) {
    int adjustedNumberOfSignificantDigits = numberOfSignificantDigits - requiredLength + bufferSize - 1;
    adjustedNumberOfSignificantDigits = adjustedNumberOfSignificantDigits < 1 ? 1 : adjustedNumberOfSignificantDigits;
    requiredLength = convertFloatToTextPrivate(f, tempBuffer, adjustedNumberOfSignificantDigits, Mode::Scientific);
  }
  requiredLength = requiredLength < bufferSize ? requiredLength : bufferSize;
  strlcpy(buffer, tempBuffer, bufferSize);
  return requiredLength;
}

template <class T>
int PrintFloat::convertFloatToTextPrivate(T f, char * buffer, int numberOfSignificantDigits, Mode mode) {
  assert(mode != Mode::Default);
  assert(numberOfSignificantDigits > 0);
  /*if (std::isinf(f)) {
    int currentChar = 0;
    if (f < 0) {
      buffer[currentChar++] = '-';
    }
    buffer[currentChar++] = 'i';
    buffer[currentChar++] = 'n';
    buffer[currentChar++] = 'f';
    buffer[currentChar] = 0;
    return currentChar;
  }*/

  if (std::isinf(f) || std::isnan(f)) {
    int currentChar = 0;
    buffer[currentChar++] = 'u';
    buffer[currentChar++] = 'n';
    buffer[currentChar++] = 'd';
    buffer[currentChar++] = 'e';
    buffer[currentChar++] = 'f';
    buffer[currentChar] = 0;
    return currentChar;
  }

  int exponentInBase10 = IEEE754<T>::exponentBase10(f);

  Mode displayMode = mode;
  if ((exponentInBase10 >= numberOfSignificantDigits || exponentInBase10 <= -numberOfSignificantDigits) && mode == Mode::Decimal) {
    displayMode = Mode::Scientific;
  }

  // Number of char available for the mantissa
  int availableCharsForMantissaWithoutSign = numberOfSignificantDigits + 1;
  int availableCharsForMantissaWithSign = f >= 0 ? availableCharsForMantissaWithoutSign : availableCharsForMantissaWithoutSign + 1;

  // Compute mantissa
  /* The number of digits in an mantissa is capped because the maximal int64_t
   * is 2^63 - 1. As our mantissa is an integer built from an int64_t, we assert
   * that we stay beyond this threshold during computation. */
  assert(availableCharsForMantissaWithoutSign - 1 < std::log10(std::pow(2.0f, 63.0f)));

  int numberOfDigitBeforeDecimal = exponentInBase10 >= 0 || displayMode == Mode::Scientific ?
                                   exponentInBase10 + 1 : 1;

  T unroundedMantissa = f * std::pow((T)10.0, (T)(availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal));
  T mantissa = std::round(unroundedMantissa);

  /* if availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal
   * is too big (or too small), mantissa is now inf. We handle this case by
   * using logarithm function. */
  if (std::isnan(mantissa) || std::isinf(mantissa)) {
    mantissa = std::round(std::pow(10, std::log10(std::fabs(f))+(T)(availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal)));
    mantissa = std::copysign(mantissa, f);
  }
  /* We update the exponent in base 10 (if 0.99999999 was rounded to 1 for
   * instance)
   * NB: the following if-condition would rather be:
   * "exponentBase10(unroundedMantissa) != exponentBase10(mantissa)",
   * however, unroundedMantissa can have a different exponent than expected
   * (ex: f = 1E13, unroundedMantissa = 99999999.99 and mantissa = 1000000000) */
  if (f != 0 && IEEE754<T>::exponentBase10(mantissa)-exponentInBase10 != availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal) {
    exponentInBase10++;
  }

  // Update the display mode if the exponent changed
  if ((exponentInBase10 >= numberOfSignificantDigits || exponentInBase10 <= -numberOfSignificantDigits) && mode == Mode::Decimal) {
    displayMode = Mode::Scientific;
  }

  int decimalMarkerPosition = exponentInBase10 < 0 || displayMode == Mode::Scientific ?
    1 : exponentInBase10+1;
  decimalMarkerPosition = f < 0 ? decimalMarkerPosition+1 : decimalMarkerPosition;

  // Correct the number of digits in mantissa after rounding
  int mantissaExponentInBase10 = exponentInBase10 > 0 || displayMode == Mode::Scientific ? availableCharsForMantissaWithoutSign - 1 : availableCharsForMantissaWithoutSign + exponentInBase10;
  if (std::floor(std::fabs((T)mantissa) * std::pow((T)10, - mantissaExponentInBase10)) > 0) {
    mantissa = mantissa/10;
  }

  int numberOfCharExponent = exponentInBase10 != 0 ? std::log10(std::fabs((T)exponentInBase10)) + 1 : 1;
  if (exponentInBase10 < 0){
    // If the exponent is < 0, we need a additional char for the sign
    numberOfCharExponent++;
  }

  // Supress the 0 on the right side of the mantissa
  Integer dividend = Integer((int64_t)std::fabs(mantissa));
  Integer quotient = Integer::Division(dividend, Integer(10)).quotient;
  Integer digit = Integer::Subtraction(dividend, Integer::Multiplication(quotient, Integer(10)));
  int minimumNumberOfCharsInMantissa = 1;
  while (digit.isZero() && availableCharsForMantissaWithoutSign > minimumNumberOfCharsInMantissa &&
      (availableCharsForMantissaWithoutSign > exponentInBase10+2 || displayMode == Mode::Scientific)) {
    mantissa = mantissa/10;
    availableCharsForMantissaWithoutSign--;
    availableCharsForMantissaWithSign--;
    dividend = quotient;
    quotient = Integer::Division(dividend, Integer(10)).quotient;
    digit = Integer::Subtraction(dividend, Integer::Multiplication(quotient, Integer(10)));
  }

  // Suppress the decimal marker if no fractional part
  if ((displayMode == Mode::Decimal && availableCharsForMantissaWithoutSign == exponentInBase10+2)
      || (displayMode == Mode::Scientific && availableCharsForMantissaWithoutSign == 2)) {
    availableCharsForMantissaWithSign--;
  }

  // Print mantissa
  assert(availableCharsForMantissaWithSign < PrintFloat::k_maxFloatBufferLength);
  PrintFloat::printBase10IntegerWithDecimalMarker(buffer, availableCharsForMantissaWithSign, Integer((int64_t)mantissa), decimalMarkerPosition);
  if (displayMode == Mode::Decimal || exponentInBase10 == 0) {
    buffer[availableCharsForMantissaWithSign] = 0;
    return availableCharsForMantissaWithSign;
  }
  // Print exponent
  assert(availableCharsForMantissaWithSign < PrintFloat::k_maxFloatBufferLength);
  buffer[availableCharsForMantissaWithSign] = Ion::Charset::Exponent;
  assert(numberOfCharExponent+availableCharsForMantissaWithSign+1 < PrintFloat::k_maxFloatBufferLength);
  PrintFloat::printBase10IntegerWithDecimalMarker(buffer+availableCharsForMantissaWithSign+1, numberOfCharExponent, Integer(exponentInBase10), -1);
  buffer[availableCharsForMantissaWithSign+1+numberOfCharExponent] = 0;
  return (availableCharsForMantissaWithSign+1+numberOfCharExponent);
}

template int PrintFloat::convertFloatToText<float>(float, char*, int, int, PrintFloat::Mode);
template int PrintFloat::convertFloatToText<double>(double, char*, int, int, PrintFloat::Mode);

}

