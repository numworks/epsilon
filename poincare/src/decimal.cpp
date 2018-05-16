#include <poincare/decimal.h>
#include <poincare/complex.h>
#include <poincare/rational.h>
#include <poincare/opposite.h>
#include <poincare/ieee754.h>
#include <assert.h>
#include <ion.h>
#include <cmath>
extern "C" {
#include <assert.h>
}

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
  return numerator;
}

Decimal::Decimal(Integer mantissa, int exponent) :
  m_mantissa(mantissa),
  m_exponent(exponent)
{
}

Decimal::Decimal(double f) {
  m_exponent = IEEE754<double>::exponentBase10(f);
  int64_t mantissaf = std::round(f * std::pow(10.0, -m_exponent+PrintFloat::k_numberOfStoredSignificantDigits+1));
  m_mantissa = Integer(mantissaf);
}

Expression::Type Decimal::type() const {
  return Type::Decimal;
}

Expression * Decimal::clone() const {
  return new Decimal(m_mantissa, m_exponent);
}

template<typename T> Expression * Decimal::templatedApproximate(Context& context, Expression::AngleUnit angleUnit) const {
  T m = m_mantissa.approximate<T>();
  int numberOfDigits = Integer::numberOfDigitsWithoutSign(m_mantissa);
  return new Complex<T>(Complex<T>::Float(m*std::pow((T)10.0, (T)(m_exponent-numberOfDigits+1))));
}

int Decimal::convertToText(char * buffer, int bufferSize, PrintFloat::Mode mode, int numberOfSignificantDigits) const {
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
  int exponent = m_exponent;
  char tempBuffer[PrintFloat::k_numberOfStoredSignificantDigits+1];
  // Round the integer if m_mantissa > 10^numberOfSignificantDigits-1
  Integer absMantissa = m_mantissa;
  absMantissa.setNegative(false);
  int numberOfDigitsInMantissa = Integer::numberOfDigitsWithoutSign(m_mantissa);
  if (numberOfDigitsInMantissa > numberOfSignificantDigits) {
    IntegerDivision d = Integer::Division(absMantissa, Integer((int64_t)std::pow(10.0, numberOfDigitsInMantissa - numberOfSignificantDigits)));
    absMantissa = d.quotient;
    if (Integer::NaturalOrder(d.remainder, Integer((int64_t)(5.0*std::pow(10.0, numberOfDigitsInMantissa-numberOfSignificantDigits-1)))) >= 0) {
      absMantissa = Integer::Addition(absMantissa, Integer(1));
      // if 9999 was rounded to 10000, we need to update exponent and mantissa
      if (Integer::numberOfDigitsWithoutSign(absMantissa) > numberOfSignificantDigits) {
        exponent++;
        absMantissa = Integer::Division(absMantissa, Integer(10)).quotient;
      }
    }
    removeZeroAtTheEnd(absMantissa);
  }
  int mantissaLength = absMantissa.writeTextInBuffer(tempBuffer, PrintFloat::k_numberOfStoredSignificantDigits+1);
  if (strcmp(tempBuffer, "undef") == 0) {
    strlcpy(buffer, tempBuffer, bufferSize);
    return mantissaLength;
  }
  /* We force scientific mode if the number of digits before the dot is superior
   * to the number of significant digits (ie with 4 significant digits,
   * 12345 -> 1.235E4 or 12340 -> 1.234E4). */
  bool forceScientificMode = mode == PrintFloat::Mode::Scientific || exponent >= numberOfSignificantDigits;
  int numberOfRequiredDigits = mantissaLength;
  if (!forceScientificMode) {
    numberOfRequiredDigits = mantissaLength > exponent ? mantissaLength : exponent;
    numberOfRequiredDigits = exponent < 0 ? 1+mantissaLength-exponent : numberOfRequiredDigits;
  }
  if (m_mantissa.isNegative()) {
    buffer[currentChar++] = '-';
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
  }
  /* Case 0: Scientific mode. Three cases:
   * - the user chooses the scientific mode
   * - the exponent is too big compared to the number of significant digits, so
   *   we force the scientific mode to avoid inventing digits
   * - the number would be too long if we print it as a natural decimal */
  if (numberOfRequiredDigits > PrintFloat::k_numberOfStoredSignificantDigits || forceScientificMode) {
    if (mantissaLength == 1) {
      currentChar += strlcpy(buffer+currentChar, tempBuffer, bufferSize-currentChar);
    } else {
      currentChar++;
      int decimalMarkerPosition = currentChar;
      if (currentChar >= bufferSize-1) { return bufferSize-1; }
      currentChar += strlcpy(buffer+currentChar, tempBuffer, bufferSize-currentChar);
      buffer[decimalMarkerPosition-1] = buffer[decimalMarkerPosition];
      buffer[decimalMarkerPosition] = '.';
    }
    if (exponent == 0) {
      return currentChar;
    }
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
    buffer[currentChar++] = Ion::Charset::Exponent;
    currentChar += Integer(exponent).writeTextInBuffer(buffer+currentChar, bufferSize-currentChar);
    return currentChar;
  }
  /* Case 1: Decimal mode */
  int deltaCharMantissa = exponent < 0 ? -exponent+1 : 0;
  strlcpy(buffer+currentChar+deltaCharMantissa, tempBuffer, bufferSize-deltaCharMantissa-currentChar);
  if (exponent < 0) {
    for (int i = 0; i <= -exponent; i++) {
      if (currentChar >= bufferSize-1) { return bufferSize-1; }
      if (i == 1) {
        buffer[currentChar++] = '.';
        continue;
      }
      buffer[currentChar++] = '0';
    }
  }
  currentChar += mantissaLength;
  if (exponent >= 0 && exponent < mantissaLength-1) {
    if (currentChar+1 >= bufferSize-1) { return bufferSize-1; }
    int decimalMarkerPosition = m_mantissa.isNegative() ? exponent + 1 : exponent;
    for (int i = currentChar-1; i > decimalMarkerPosition; i--) {
      buffer[i+1] = buffer[i];
    }
    buffer[decimalMarkerPosition+1] = '.';
    currentChar++;
  }
  if (exponent >= 0 && exponent > mantissaLength-1) {
    int endMarkerPosition = m_mantissa.isNegative() ? exponent+1 : exponent;
    for (int i = currentChar-1; i < endMarkerPosition; i++) {
      if (currentChar+1 >= bufferSize-1) { return bufferSize-1; }
      buffer[currentChar++] = '0';
    }
  }
  buffer[currentChar] = 0;
  return currentChar;
}

int Decimal::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  return convertToText(buffer, bufferSize, PrintFloat::Mode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits);
}

bool Decimal::needParenthesisWithParent(const Expression * e) const {
  if (sign() == Sign::Positive) {
    return false;
  }
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return e->isOfType(types, 7);
}

ExpressionLayout * Decimal::privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const {
  char buffer[k_maxBufferSize];
  int numberOfChars = convertToText(buffer, k_maxBufferSize, floatDisplayMode, PrintFloat::k_numberOfStoredSignificantDigits);
  return LayoutEngine::createStringLayout(buffer, numberOfChars);
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
  Integer numerator = m_mantissa;
  removeZeroAtTheEnd(numerator);
  int numberOfDigits = Integer::numberOfDigitsWithoutSign(numerator);
  Integer denominator = Integer(1);
  if (m_exponent >= numberOfDigits-1) {
    numerator = Integer::Multiplication(numerator, Integer::Power(Integer(10), Integer(m_exponent-numberOfDigits+1)));
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

}
