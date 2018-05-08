#include <poincare/decimal.h>
#include <poincare/complex.h>
#include <poincare/rational.h>
#include <poincare/opposite.h>
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

void removeZeroAtTheEnd(Integer & i) {
  if (i.isZero()) {
    return;
  }
  Integer base = Integer(10);
  IntegerDivision d = Integer::Division(i, base);
  while (d.remainder.isZero()) {
    i = d.quotient;
    d = Integer::Division(i, base);
  }
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
  removeZeroAtTheEnd(numerator);
  return numerator;
}

Decimal::Decimal(Integer mantissa, int exponent) :
  m_mantissa(mantissa),
  m_exponent(exponent)
{
}

Decimal::Decimal(double f) {
  double logBase10 = f != 0 ? std::log10(std::fabs(f)) : 0;
  int exponentInBase10 = std::floor(logBase10);
  /* Correct the exponent in base 10: sometines the exact log10 of f is 6.999999
   * but is stored as 7 in hardware. We catch these cases here. */
  if (f != 0 && logBase10 == (int)logBase10 && std::fabs(f) < std::pow(10, logBase10)) {
    exponentInBase10--;
  }
  double m = f*std::pow(10, (double)-exponentInBase10); // TODO: hangle exponentInBase10 is too big! mantissa is nan
  m = m * std::pow(10, (double)(k_doublePrecision-1));
  int64_t integerMantissa = std::round(m);
  /* If m > 999999999999999.5, the mantissa stored will be 1 (as we keep only
   * 15 significative numbers from double. In that case, the exponent must be
   * increment as well. */
  if (m >= k_biggestMantissaFromDouble+0.5) {
    exponentInBase10++;
  }
  m_mantissa = Integer(integerMantissa);
  removeZeroAtTheEnd(m_mantissa);
  m_exponent = exponentInBase10;
}

Expression::Type Decimal::type() const {
  return Type::Decimal;
}

Expression * Decimal::clone() const {
  return new Decimal(m_mantissa, m_exponent);
}

template<typename T> Expression * Decimal::templatedApproximate(Context& context, Expression::AngleUnit angleUnit) const {
  T m = m_mantissa.approximate<T>();
  int numberOfDigits = numberOfDigitsInMantissaWithoutSign();
  return new Complex<T>(Complex<T>::Float(m*std::pow((T)10.0, (T)(m_exponent-numberOfDigits+1))));
}

int Decimal::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
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
  char tempBuffer[200];
  int mantissaLength = m_mantissa.writeTextInBuffer(tempBuffer, 200);
  if (strcmp(tempBuffer, "undef") == 0) {
    strlcpy(buffer, tempBuffer, bufferSize);
    return mantissaLength;
  }
  int nbOfDigitsInMantissaWithoutSign = numberOfDigitsInMantissaWithoutSign();
  int numberOfRequiredDigits = nbOfDigitsInMantissaWithoutSign > m_exponent ? nbOfDigitsInMantissaWithoutSign : m_exponent;
  numberOfRequiredDigits = m_exponent < 0 ? 1+nbOfDigitsInMantissaWithoutSign-m_exponent : numberOfRequiredDigits;
  /* Case 0: the number would be too long if we print it as a natural decimal */
  if (numberOfRequiredDigits > k_maxLength) {
    if (nbOfDigitsInMantissaWithoutSign == 1) {
      currentChar += strlcpy(buffer, tempBuffer, bufferSize);
    } else {
      currentChar++;
      if (currentChar >= bufferSize-1) { return bufferSize-1; }
      currentChar += strlcpy(buffer+currentChar, tempBuffer, bufferSize-currentChar);
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
  int deltaCharMantissa = m_exponent < 0 ? -m_exponent+1 : 0;
  strlcpy(buffer+deltaCharMantissa, tempBuffer, bufferSize-deltaCharMantissa);
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
  char tempChar = 0;
  int tempCharPosition = 0;
  if (m_mantissa.isNegative()) {
    currentChar--;
    tempChar = buffer[currentChar];
    tempCharPosition = currentChar;
  }
  currentChar += mantissaLength;
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

bool Decimal::needParenthesisWithParent(const Expression * e) const {
  if (sign() == Sign::Positive) {
    return false;
  }
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return e->isOfType(types, 7);
}

ExpressionLayout * Decimal::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  char buffer[255];
  int numberOfChars = writeTextInBuffer(buffer, 255);
  return new StringLayout(buffer, numberOfChars);
}

Expression * Decimal::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
  // Do not reduce decimal to rational if the exponent is too big or too small.
  if (m_exponent > k_maxDoubleExponent || m_exponent < -k_maxDoubleExponent) {
    return this; // TODO: return new Infinite() ? new Rational(0) ?
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

Expression * Decimal::shallowBeautify(Context & context, AngleUnit angleUnit) {
  if (m_mantissa.isNegative()) {
    m_mantissa.setNegative(false);
    Opposite * o = new Opposite(this, true);
    return replaceWith(o, true);
  }
  return this;
}

int Decimal::simplificationOrderSameType(const Expression * e, bool canBeInterrupted) const {
  assert(e->type() == Type::Decimal);
  const Decimal * other = static_cast<const Decimal *>(e);
  if (sign() == Sign::Negative && other->sign() == Sign::Positive) {
    return -1;
  }
  if (sign() == Sign::Positive && other->sign() == Sign::Negative) {
    return 1;
  }
  assert(sign() == other->sign());
  int unsignedComparison = 0;
  if (exponent() < other->exponent()) {
    unsignedComparison = -1;
  } else if (exponent() > other->exponent()) {
    unsignedComparison = 1;
  } else {
    assert(exponent() == other->exponent());
    unsignedComparison = Integer::NaturalOrder(mantissa(), other->mantissa());
  }
  return ((int)sign())*unsignedComparison;
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
