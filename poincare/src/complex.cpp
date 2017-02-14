#include <poincare/complex.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
}
#include "layout/string_layout.h"
#include <ion.h>

namespace Poincare {

Complex::Complex(float a, float b, bool polar) :
  m_a(a),
  m_b(b),
  m_numberOfSignificantDigits(7)
{
  if (polar) {
    m_a = a * cosf(b);
    m_b = a * sinf(b);
  }
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

Complex::Complex(const char * integralPart, int integralPartLength, bool integralNegative,
    const char * fractionalPart, int fractionalPartLength,
    const char * exponent, int exponentLength, bool exponentNegative) {
  float i = digitsToFloat(integralPart, integralPartLength);
  float j = digitsToFloat(fractionalPart, fractionalPartLength);
  float l = setSign(digitsToFloat(exponent, exponentLength), exponentNegative);

  m_a = setSign((i + j*powf(10.0f, -ceilf(fractionalPartLength)))* powf(10.0f, l), integralNegative);
  m_b = 0.0f;
  m_numberOfSignificantDigits = 7;
}

void Complex::setNumberOfSignificantDigits(int numberOfDigits) {
  m_numberOfSignificantDigits = numberOfDigits;
}

Expression * Complex::clone() const {
  return new Complex(m_a, m_b);
}

float Complex::approximate(Context& context, AngleUnit angleUnit) const {
  if (m_b == 0.0f) {
    return m_a;
  }
  return NAN;
}

Expression * Complex::evaluate(Context& context, AngleUnit angleUnit) const {
  return clone();
}

Expression::Type Complex::type() const {
  return Type::Complex;
}

ExpressionLayout * Complex::createLayout(DisplayMode displayMode) const {
  char buffer[k_maxComplexBufferLength];
  int numberOfChars = convertComplexToText(buffer, k_maxComplexBufferLength, displayMode);
  return new StringLayout(buffer, numberOfChars);
}

int Complex::writeTextInBuffer(char * buffer, int bufferSize) {
  return convertComplexToText(buffer, bufferSize, DisplayMode::Auto);
}

float Complex::a() {
  return m_a;
}

float Complex::b() {
  return m_b;
}

float Complex::r() {
  return sqrtf(m_a*m_a + m_b*m_b);
}

float Complex::th() {
  float result = atanf(m_b/m_a) + M_PI;
  if (m_a >= 0.0f) {
    result = atanf(m_b/m_a);
  }
  if (result > M_PI + FLT_EPSILON) {
    result = result - 2.0f*M_PI;
  }
  return result;
}

float Complex::absoluteValue() {
  return sqrtf(m_a*m_a+m_b*m_b);
}

int Complex::convertFloatToText(float f, char * buffer, int bufferSize,
    int numberOfSignificantDigits, DisplayMode mode) {
  char tempBuffer[k_maxFloatBufferLength];
  int requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits, mode);
  /* if the required buffer size overflows the buffer size, we first force the
   * display mode to scientific and decrease the number of significant digits to
   * fit the buffer size. If the buffer size is still to small, we only write
   * the beginning of the float and truncate it (which can result in a non sense
   * text) */
  if (mode == DisplayMode::Auto && requiredLength >= bufferSize) {
    requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits, DisplayMode::Scientific);
  }
  if (requiredLength >= bufferSize) {
    requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits - requiredLength + bufferSize - 1, DisplayMode::Scientific);
  }
  requiredLength = requiredLength < bufferSize ? requiredLength : bufferSize;
  strlcpy(buffer, tempBuffer, bufferSize);
  return requiredLength;
}

int Complex::convertComplexToText(char * buffer, int bufferSize, DisplayMode displayMode) const {
  int numberOfChars = 0;
  if (m_a != 0.0f || m_b == 0.0f) {
    numberOfChars = convertFloatToText(m_a, buffer, bufferSize, m_numberOfSignificantDigits, displayMode);
    if (m_b > 0.0f && bufferSize > numberOfChars+1) {
      buffer[numberOfChars++] = '+';
      // Ensure that the string is null terminated even if buffer size is to small
      buffer[numberOfChars] = 0;
    }
  }
  if (m_b != 1.0f && m_b != -1.0f && m_b != 0.0f) {
    numberOfChars += convertFloatToText(m_b, buffer+numberOfChars, bufferSize-numberOfChars, m_numberOfSignificantDigits, displayMode);
    buffer[numberOfChars++] = '*';
  }
  if (m_b == -1.0f && bufferSize > numberOfChars+1) {
    buffer[numberOfChars++] = '-';
  }
  if (m_b != 0.0f && bufferSize > numberOfChars+1) {
    buffer[numberOfChars++] = Ion::Charset::SmallIota;
    buffer[numberOfChars] = 0;
  }
  return numberOfChars;
}


int Complex::convertFloatToTextPrivate(float f, char * buffer, int numberOfSignificantDigits, DisplayMode mode) {
  if (isinf(f)) {
    buffer[0] = f > 0 ? '+' : '-';
    buffer[1] = 'I';
    buffer[2] = 'n';
    buffer[3] = 'f';
    buffer[4] = 0;
    return 5;
  }

  if (isnan(f)) {
    buffer[0] = 'N';
    buffer[1] = 'a';
    buffer[2] = 'N';
    buffer[3] = 0;
    return 4;
  }

  float logBase10 = f != 0.0f ? log10f(fabsf(f)) : 0;
  int exponentInBase10 = logBase10;
  if ((int)f == 0 && logBase10 != exponentInBase10) {
    /* For floats < 0, the exponent in base 10 is the inferior integer part of
     * log10(float). We thus decrement the exponent for float < 0 whose exponent
     * is not an integer. */
    exponentInBase10--;
  }

  DisplayMode displayMode = mode;
  if ((exponentInBase10 >= numberOfSignificantDigits || exponentInBase10 <= -numberOfSignificantDigits) && mode == DisplayMode::Auto) {
    displayMode = DisplayMode::Scientific;
  }

  int decimalMarkerPosition = exponentInBase10 < 0 || displayMode == DisplayMode::Scientific ?
    1 : exponentInBase10+1;

  // Number of char available for the mantissa
  int availableCharsForMantissaWithoutSign = numberOfSignificantDigits + 1;
  int availableCharsForMantissaWithSign = f >= 0 ? availableCharsForMantissaWithoutSign : availableCharsForMantissaWithoutSign + 1;

  // Compute mantissa
  /* The number of digits in an integer is capped because the maximal integer is
   * 2^31 - 1. As our mantissa is an integer, we assert that we stay beyond this
   * threshold during computation. */
  int numberMaximalOfCharsInInteger = log10f(powf(2, 31));
  assert(availableCharsForMantissaWithoutSign - 1 < numberMaximalOfCharsInInteger);
  int numberOfDigitBeforeDecimal = exponentInBase10 >= 0 || displayMode == DisplayMode::Scientific ?
                                   exponentInBase10 + 1 : 1;
  int mantissa = roundf(f * powf(10, availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal));
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
  if (displayMode == DisplayMode::Auto && availableCharsForMantissaWithoutSign == exponentInBase10+2) {
    availableCharsForMantissaWithSign--;
  }

  // Print mantissa
  printBase10IntegerWithDecimalMarker(buffer, availableCharsForMantissaWithSign, mantissa, decimalMarkerPosition);
  if (displayMode == DisplayMode::Auto) {
    buffer[availableCharsForMantissaWithSign] = 0;
    return availableCharsForMantissaWithSign;
  }
  // Print exponent
  buffer[availableCharsForMantissaWithSign] = Ion::Charset::Exponent;
  printBase10IntegerWithDecimalMarker(buffer+availableCharsForMantissaWithSign+1, numberOfCharExponent, exponentInBase10, -1);
  buffer[availableCharsForMantissaWithSign+1+numberOfCharExponent] = 0;
  return (availableCharsForMantissaWithSign+1+numberOfCharExponent);
}

void Complex::printBase10IntegerWithDecimalMarker(char * buffer, int bufferSize,  int i, int decimalMarkerPosition) {
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

}

