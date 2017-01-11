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
  m_numberOfSignificantDigits(7)
{
}

static inline float setSign(float f, bool negative) {
  if (negative) {
    return -f;
  }
  return f;
}

float digitsToFloat(const char * digits, int length) {
  if (digits == nullptr) {
    return 0.0f;
  }
  float result = 0.0f;
  const char * digit = digits;
  for (int i = 0; i < length; i++) {
    result = 10.0f * result;
    result += *digit-'0';
    digit++;
  }
  return result;
}

Float::Float(const char * integralPart, int integralPartLength, bool integralNegative,
    const char * fractionalPart, int fractionalPartLength,
    const char * exponent, int exponentLength, bool exponentNegative) {
  float i = digitsToFloat(integralPart, integralPartLength);
  float j = digitsToFloat(fractionalPart, fractionalPartLength);
  float l = setSign(digitsToFloat(exponent, exponentLength), exponentNegative);

  m_float = setSign((i + j*powf(10.0f, -ceilf(fractionalPartLength)))* powf(10.0f, l), integralNegative);
  m_numberOfSignificantDigits = 7;
}

void Float::setNumberOfSignificantDigits(int numberOfDigits) {
  m_numberOfSignificantDigits = numberOfDigits;
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
  convertFloatToText(buffer, k_maxBufferLength, m_numberOfSignificantDigits);
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
  return convertFloatToText(buffer, bufferSize, m_numberOfSignificantDigits);
}

int Float::convertFloatToText(char * buffer, int bufferSize,
    int numberOfSignificantDigits, DisplayMode mode) const {
  char tempBuffer[k_maxBufferLength];
  int requiredLength = convertFloatToTextPrivate(tempBuffer, numberOfSignificantDigits, mode);
  /* if the required buffer size overflows the buffer size, we first force the
   * display mode to scientific and decrease the number of significant digits to
   * fit the buffer size. If the buffer size is still to small, we only write
   * the beginning of the float and truncate it (which can result in a non sense
   * text) */
  if (mode == DisplayMode::Decimal && requiredLength >= bufferSize) {
    requiredLength = convertFloatToTextPrivate(tempBuffer, numberOfSignificantDigits, DisplayMode::Scientific);
  }
  if (requiredLength >= bufferSize) {
    requiredLength = convertFloatToTextPrivate(tempBuffer, numberOfSignificantDigits - requiredLength + bufferSize - 1, DisplayMode::Scientific);
  }
  requiredLength = requiredLength < bufferSize ? requiredLength : bufferSize;
  strlcpy(buffer, tempBuffer, bufferSize);
  return requiredLength;
}

int Float::convertFloatToTextPrivate(char * buffer, int numberOfSignificantDigits, DisplayMode mode) const {
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
  if ((exponentInBase10 >= numberOfSignificantDigits || exponentInBase10 <= -numberOfSignificantDigits) && mode == DisplayMode::Decimal) {
    displayMode = DisplayMode::Scientific;
  }

  int decimalMarkerPosition = exponentInBase10 < 0 || displayMode == DisplayMode::Scientific ?
    1 : exponentInBase10+1;

  // Number of char available for the mantissa
  int availableCharsForMantissaWithoutSign = numberOfSignificantDigits + 1;
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
