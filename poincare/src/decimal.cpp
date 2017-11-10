#include <poincare/decimal.h>
#include <poincare/complex.h>
#include <poincare/rational.h>
#include <assert.h>
#include <ion.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

#include "layout/string_layout.h"

namespace Poincare {

int Decimal::exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength, bool exponentNegative) {
  int base = 10;
  int exp = 0;
  for (int i = 0; i < exponentLength; i++) {
    exp *= base;
    exp += *exponent-'0';
    exponent++;
  }
  if (exponentNegative) {
    exp = -exp;
  }
  const char * integralPartEnd = integralPart + integralPartLength;
  if (integralPart != nullptr) {
    while (*integralPart == '0' && integralPart < integralPartEnd) {
      integralPart++;
    }
  }
  exp += integralPartEnd-integralPart-1;
  if (integralPart == integralPartEnd) {
    const char * fractionalPartEnd = fractionalPart + fractionalPartLength;
    if (fractionalPart != nullptr) {
      while (*fractionalPart == '0' && fractionalPart < fractionalPartEnd) {
        fractionalPart++;
        exp--;
      }
    }
    if (fractionalPart == fractionalPartEnd) {
      exp += fractionalPartLength+1;
    }
  }
  return exp;
}

Integer Decimal::mantissa(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, bool negative) {
  Integer zero = Integer(0);
  Integer base = Integer(10);
  Integer numerator = Integer(integralPart, negative);
  for (int i = 0; i < fractionalPartLength; i++) {
    numerator = Integer::Multiplication(numerator, base);
    numerator = Integer::Addition(numerator, Integer(*fractionalPart-'0'));
    fractionalPart++;
  }
  if (numerator.isZero()) {
    return numerator;
  }
  IntegerDivision d = Integer::Division(numerator, base);
  while (d.remainder.isZero()) {
    numerator = d.quotient;
    d = Integer::Division(numerator, base);
  }
  return numerator;
}

Decimal::Decimal(Integer mantissa, int exponent) :
  m_mantissa(mantissa),
  m_exponent(exponent)
{
}

Expression::Type Decimal::type() const {
  return Type::Decimal;
}

Expression * Decimal::clone() const {
  return new Decimal(m_mantissa, m_exponent);
}

template<typename T> Complex<T> * Decimal::templatedEvaluate(Context& context, Expression::AngleUnit angleUnit) const {
  T m = m_mantissa.approximate<T>();
  int numberOfDigits = numberOfDigitsInMantissaWithoutSign();
  return new Complex<T>(Complex<T>::Float(m*std::pow((T)10.0, (T)(m_exponent-numberOfDigits+1))));
}

int Decimal::writeTextInBuffer(char * buffer, int bufferSize) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize-1) { return bufferSize-1; }
  if (m_mantissa.isZero()) {
    buffer[currentChar++] = '0';
    buffer[currentChar] = 0;
    return currentChar;
  }
  int nbOfDigitsInMantissaWithoutSign = numberOfDigitsInMantissaWithoutSign();
  int numberOfRequiredDigits = nbOfDigitsInMantissaWithoutSign > m_exponent ? nbOfDigitsInMantissaWithoutSign : m_exponent;
  numberOfRequiredDigits = m_exponent < 0 ? 1+nbOfDigitsInMantissaWithoutSign-m_exponent : numberOfRequiredDigits;
  /* Case 0: the number would be too long if we print it as a natural decimal */
  if (numberOfRequiredDigits > k_maxLength) {
    if (nbOfDigitsInMantissaWithoutSign == 1) {
      currentChar +=m_mantissa.writeTextInBuffer(buffer, bufferSize);
    } else {
      currentChar++;
      if (currentChar >= bufferSize-1) { return bufferSize-1; }
      currentChar += m_mantissa.writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
      int decimalMarkerPosition = 1;
      if (buffer[1] == '-') {
        decimalMarkerPosition++;
        buffer[0] = buffer[1];
      }
      buffer[decimalMarkerPosition-1] = buffer[decimalMarkerPosition];
      buffer[decimalMarkerPosition] = '.';
    }
    if (m_exponent == 0) {
      return currentChar;
    }
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
    buffer[currentChar++] = Ion::Charset::Exponent;
    currentChar += Integer(m_exponent).writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
    return currentChar;
  }
  /* Case 2: Print a natural decimal number */
  if (m_mantissa.isNegative()) {
    buffer[currentChar++] = '-';
  }
  if (m_exponent < 0) {
    for (int i = 0; i <= -m_exponent; i++) {
      if (currentChar >= bufferSize-1) { return bufferSize-1; }
      if (i == 1) {
        buffer[currentChar++] = '.';
        continue;
      }
      buffer[currentChar++] = '0';
    }
  }
  /* If mantissa is negative, m_mantissa.writeTextInBuffer is going to add an
   * unwanted '-' in place of the temp char. We store it to replace it back
   * after calling m_mantissa.writeTextInBuffer. */
  char tempChar;
  int tempCharPosition;
  if (m_mantissa.isNegative()) {
    currentChar--;
    tempChar = buffer[currentChar];
    tempCharPosition = currentChar;
  }
  currentChar += m_mantissa.writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
  if (m_mantissa.isNegative()) { // replace the temp char back
    buffer[tempCharPosition] = tempChar;
  }
  int currentExponent = m_mantissa.isNegative() ? currentChar-2 : currentChar-1;
  if (m_exponent >= 0 && m_exponent < currentExponent) {
    if (currentChar+1 >= bufferSize-1) { return bufferSize-1; }
    int decimalMarkerPosition = m_mantissa.isNegative() ? m_exponent +1 : m_exponent;
    for (int i = currentChar-1; i > decimalMarkerPosition; i--) {
      buffer[i+1] = buffer[i];
    }
    buffer[decimalMarkerPosition+1] = '.';
    currentChar++;
  }
  if (m_exponent >= 0 && m_exponent > currentExponent) {
    int decimalMarkerPosition = m_mantissa.isNegative() ? m_exponent+1 : m_exponent;
    for (int i = currentChar-1; i < decimalMarkerPosition; i++) {
      if (currentChar+1 >= bufferSize-1) { return bufferSize-1; }
      buffer[currentChar++] = '0';
    }
  }
  buffer[currentChar] = 0;
  return currentChar;
}

ExpressionLayout * Decimal::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  char buffer[255];
  int numberOfChars = writeTextInBuffer(buffer, 255);
  return new StringLayout(buffer, numberOfChars);
}

Expression * Decimal::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  int numberOfDigits = numberOfDigitsInMantissaWithoutSign();
  Integer numerator = m_mantissa;
  Integer denominator = Integer(1);
  if (m_exponent >= numberOfDigits-1) {
    numerator = Integer::Multiplication(m_mantissa, Integer::Power(Integer(10), Integer(m_exponent-numberOfDigits+1)));
  } else {
    denominator = Integer::Power(Integer(10), Integer(numberOfDigits-1-m_exponent));
  }
  return replaceWith(new Rational(numerator, denominator), true);
}

int Decimal::simplificationOrderSameType(const Expression * e) const {
  // We should not get there are decimal are turned into Rational before simplification
  assert(false);
  return 0;
}

int Decimal::numberOfDigitsInMantissaWithoutSign() const {
  int numberOfDigits = 1;
  Integer mantissaCopy = m_mantissa;
  mantissaCopy.setNegative(false);
  IntegerDivision d = Integer::Division(mantissaCopy, Integer(10));
  while (!d.quotient.isZero()) {
    mantissaCopy = d.quotient;
    d = Integer::Division(mantissaCopy, Integer(10));
    numberOfDigits++;
  }
  return numberOfDigits;
}

}
