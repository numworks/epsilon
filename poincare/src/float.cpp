extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
}
#include "layout/string_layout.h"
#include <poincare/float.h>


Float::Float(float f) :
  m_float(f),
  m_numberOfDigitsInMantissa(7)
{
}

static inline float setSign(float f, bool negative) {
  if (negative) {
    return -f;
  }
  return f;
}

/* Handles non-NULL terminated string */
float digitsToFloat(const char * digits) {
  if (digits == nullptr) {
    return 0.0f;
  }
  float result = 0.0f;
  const char * digit = digits;
  while (*digit >= '0' && *digit <= '9') {
    result = 10.0f * result;
    result += *digit-'0';
    digit++;
  }
  return result;
}

int numberOfDigits(const char * digits) {
  if (digits == nullptr) {
    return 0;
  }
  int result = 0;
  const char * digit = digits;
  while (*digit >= '0' && *digit <= '9') {
    result++;
    digit++;
  }
  return result;
}

Float::Float(const char * integralPart, bool integralNegative,
    const char * fractionalPart,
    const char * exponent, bool exponentNegative) {
  /* Caution: all the const char * are NOT guaranteed to be NULL-terminated!
   * Indeed, this code is called by Bison to build Floats from user input.
   * While Flex guarantees that yytext is NULL-terminated when building tokens,
   * it does so by temporarily swapping in a NULL terminated in the input text.
   * Of course that hack has vanished when the pointer is fed into Bison. */

  float i = setSign(digitsToFloat(integralPart), integralNegative);
  float j = digitsToFloat(fractionalPart);
  float k = numberOfDigits(fractionalPart);
  float l = setSign(digitsToFloat(exponent), exponentNegative);

  m_float =
  (i + j*powf(10.0f, -ceilf(k)))
    * powf(10.0f, l);
  if (j <= 0) {
    m_float = i * powf(10.0f, l);
  }
  m_numberOfDigitsInMantissa = 7;
}

void Float::setNumberOfDigitsInMantissa(int numberOfDigits) {
  m_numberOfDigitsInMantissa = numberOfDigits;
}

Expression * Float::clone() const {
  return new Float(m_float);
}

float Float::approximate(Context& context) const {
  return m_float;
}

Expression * Float::evaluate(Context& context) const {
  return clone();
}

Expression::Type Float::type() const {
  return Type::Float;
}

ExpressionLayout * Float::createLayout() const {
  char buffer[k_maxBufferLength];
  convertFloatToText(buffer, k_maxBufferLength, m_numberOfDigitsInMantissa);
  int size = 0;
  while (buffer[size] != 0) {
    size++;
  }
  return new StringLayout(buffer, size);
}

bool Float::valueEquals(const Expression * e) const {
  assert(e->type() == Expression::Type::Float);
  return m_float == ((Float *)e)->m_float;
}

int Float::writeTextInBuffer(char * buffer, int bufferSize) {
  return convertFloatToText(buffer, bufferSize, m_numberOfDigitsInMantissa);
}

int Float::convertFloatToText(char * buffer, int bufferSize,
    int numberOfDigitsInMantissa, DisplayMode mode) const {
  if (isinf(m_float)) {
    buffer[0] = m_float > 0 ? '+' : '-';
    buffer[1] = 'I';
    buffer[2] = 'n';
    buffer[3] = 'f';
    buffer[4] = 0;
    return 5;
  }

  if (isnan(m_float)) {
    buffer[0] = 'N';
    buffer[1] = 'a';
    buffer[2] = 'N';
    buffer[3] = 0;
    return 4;
  }

  float logBase10 = m_float != 0.0f ? log10f(fabsf(m_float)) : 0;
  int exponentInBase10 = logBase10;
  if ((int)m_float == 0 && logBase10 != exponentInBase10) {
    /* For floats < 0, the exponent in base 10 is the inferior integer part of
     * log10(float). We thus decrement the exponent for float < 0 whose exponent
     * is not an integer. */
    exponentInBase10--;
  }

  DisplayMode displayMode = mode;
  if ((exponentInBase10 >= numberOfDigitsInMantissa || exponentInBase10 <= -numberOfDigitsInMantissa) && mode == DisplayMode::Decimal) {
    displayMode = DisplayMode::Scientific;
    numberOfDigitsInMantissa = numberOfDigitsInMantissa-2;
  }

  /* We here assert that the buffer is long enough to display with the right
   * number of digits in the mantissa. If numberOfDigitsInMantissa = 7, the
   * worst case has the form -1.999999e-38 (7+6+1 char) for the scientific mode
   * and -1.999999 (7+2+1 char) for the decimal mode. */
  if ((bufferSize <= 6 + numberOfDigitsInMantissa && displayMode == DisplayMode::Scientific) ||
      (bufferSize <= 2 + numberOfDigitsInMantissa && displayMode == DisplayMode::Decimal)) {
    buffer[0] = 0;
    return 0;
  }

  int decimalMarkerPosition = exponentInBase10 < 0 || displayMode == DisplayMode::Scientific ?
    1 : exponentInBase10+1;

  // Number of char available for the mantissa
  int availableCharsForMantissaWithoutSign = numberOfDigitsInMantissa + 1;
  int availableCharsForMantissaWithSign = m_float >= 0 ? availableCharsForMantissaWithoutSign : availableCharsForMantissaWithoutSign + 1;

  // Compute mantissa
  /* The number of digits in an integer is capped because the maximal integer is
   * 2^31 - 1. As our mantissa is an integer, we assert that we stay beyond this
   * threshold during computation. */
  int numberMaximalOfCharsInInteger = log10f(powf(2, 31));
  assert(availableCharsForMantissaWithoutSign - 1 < numberMaximalOfCharsInInteger);
  int numberOfDigitBeforeDecimal = exponentInBase10 >= 0 || displayMode == DisplayMode::Scientific ?
                                   exponentInBase10 + 1 : 1;
  int mantissa = roundf(m_float * powf(10, availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal));
  // Correct the number of digits in mantissa after rounding
  int mantissaExponentInBase10 = exponentInBase10 > 0 || displayMode == DisplayMode::Scientific ? availableCharsForMantissaWithoutSign - 1 : availableCharsForMantissaWithoutSign + exponentInBase10;
  if ((int)(mantissa * powf(10, - mantissaExponentInBase10)) > 0) {
    mantissa = mantissa/10;
    exponentInBase10++;
  }

  int numberOfCharExponent = exponentInBase10 != 0 ? log10f(fabsf((float)exponentInBase10)) + 1 : 1;
  if (exponentInBase10 < 0){
    // If the exponent is < 0, we need a additional char for the sign
    numberOfCharExponent++;
  }

  // Supress the 0 on the right side of the mantissa
  int dividend = fabsf((float)mantissa);
  int quotien = dividend/10;
  int digit = dividend - quotien*10;
  int minimumNumberOfCharsInMantissa = displayMode == DisplayMode::Scientific ? 3 : 1;
  while (digit == 0 && availableCharsForMantissaWithSign > minimumNumberOfCharsInMantissa &&
      (availableCharsForMantissaWithoutSign > exponentInBase10+2 || displayMode == DisplayMode::Scientific)) {
    mantissa = mantissa/10;
    availableCharsForMantissaWithoutSign--;
    availableCharsForMantissaWithSign--;
    dividend = quotien;
    quotien = dividend/10;
    digit = dividend - quotien*10;
  }

  // Suppress the decimal marker if no fractional part
  if (displayMode == DisplayMode::Decimal && availableCharsForMantissaWithoutSign == exponentInBase10+2) {
    availableCharsForMantissaWithSign--;
  }

  // Print mantissa
  printBase10IntegerWithDecimalMarker(buffer, availableCharsForMantissaWithSign, mantissa, decimalMarkerPosition);
  if (displayMode == DisplayMode::Decimal) {
    buffer[availableCharsForMantissaWithSign] = 0;
    return availableCharsForMantissaWithSign;
  }
  // Print exponent
  buffer[availableCharsForMantissaWithSign] = 'E';
  printBase10IntegerWithDecimalMarker(buffer+availableCharsForMantissaWithSign+1, numberOfCharExponent, exponentInBase10, -1);
  buffer[availableCharsForMantissaWithSign+1+numberOfCharExponent] = 0;
  return (availableCharsForMantissaWithSign+1+numberOfCharExponent);
}

void Float::printBase10IntegerWithDecimalMarker(char * buffer, int bufferSize,  int i, int decimalMarkerPosition) {
  /* The decimal marker position is always preceded by a char, thus, it is never
   * in first position. When called by convertFloatToText, the buffer length is
   * always > 0 as we asserted a minimal number of available chars. */
  assert(bufferSize > 0 && decimalMarkerPosition != 0);
  int endChar = bufferSize - 1, startChar = 0;
  int dividend = i, digit = 0, quotien = 0;
  if (i < 0) {
    buffer[startChar++] = '-';
    dividend = -i;
    decimalMarkerPosition += 1;
  }
  /* This loop acts correctly as we asserted the endChar >= 0 and
   * decimalMarkerPosition != 0 */
  do {
    if (endChar == decimalMarkerPosition) {
      buffer[endChar--] = '.';
    }
    quotien = dividend/10;
    digit = dividend - quotien*10;
    buffer[endChar--] = '0'+digit;
    dividend = quotien;
  }  while (endChar >= startChar);

}
