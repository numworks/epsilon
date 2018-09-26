#include <poincare/decimal.h>
#include <poincare/rational.h>
#include <poincare/opposite.h>
#include <poincare/infinity.h>
#include <poincare/layout_helper.h>
#include <poincare/ieee754.h>
#include <assert.h>
#include <ion.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

void removeZeroAtTheEnd(Integer * i) {
  if (i->isZero()) {
    return;
  }
  Integer base = Integer(10);
  IntegerDivision d = Integer::Division(*i, base);
  while (d.remainder.isZero()) {
    *i = d.quotient;
    d = Integer::Division(*i, base);
  }
  assert(!i->isInfinity());
}

void DecimalNode::setValue(const native_uint_t * mantissaDigits, uint8_t mantissaSize, int exponent, bool negative) {
  m_negative = negative;
  m_exponent = exponent;
  m_numberOfDigitsInMantissa = mantissaSize;
  memcpy(m_mantissa, mantissaDigits, mantissaSize*sizeof(native_uint_t));
}

Integer DecimalNode::signedMantissa() const {
  return Integer::BuildInteger((native_uint_t *)m_mantissa, m_numberOfDigitsInMantissa, m_negative);
}

Integer DecimalNode::unsignedMantissa() const {
  return Integer::BuildInteger((native_uint_t *)m_mantissa, m_numberOfDigitsInMantissa, false);
}

size_t DecimalNode::size() const {
  return sizeof(DecimalNode)+ sizeof(native_uint_t)*m_numberOfDigitsInMantissa;
}

Expression DecimalNode::setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  return Decimal(this).setSign(s, context, angleUnit);
}

int DecimalNode::simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const {
  assert(e->type() == Type::Decimal);
  const DecimalNode * other = static_cast<const DecimalNode *>(e);
  if (m_negative && !other->m_negative) {
    return -1;
  }
  if (!m_negative && other->m_negative) {
    return 1;
  }
  assert(m_negative == other->m_negative);
  int unsignedComparison = 0;
  if (m_exponent < other->m_exponent) {
    unsignedComparison = -1;
  } else if (m_exponent > other->m_exponent) {
    unsignedComparison = 1;
  } else {
    assert(m_exponent == other->m_exponent);
    assert(exponent() == other->exponent());
    double approx0 = templatedApproximate<double>();
    double approx1 = other->templatedApproximate<double>();
    return (approx0 == approx1 ? 0 : (approx0 < approx1 ? -1 : 1));
  }
  return ((int)sign())*unsignedComparison;
}

Expression DecimalNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  return Decimal(this).shallowReduce(context, angleUnit);
}

Expression DecimalNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Decimal(this).shallowBeautify(context, angleUnit);
}

Layout DecimalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char buffer[k_maxBufferSize];
  int numberOfChars = convertToText(buffer, k_maxBufferSize, floatDisplayMode, numberOfSignificantDigits);
  return LayoutHelper::String(buffer, numberOfChars);
}

int DecimalNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return convertToText(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

int DecimalNode::convertToText(char * buffer, int bufferSize, Preferences::PrintFloatMode mode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize-1) { return bufferSize-1; }
  if (unsignedMantissa().isZero()) {
    buffer[currentChar++] = '0';
    buffer[currentChar] = 0;
    return currentChar;
  }
  int exponent = m_exponent;
  char tempBuffer[PrintFloat::k_numberOfStoredSignificantDigits+1];
  // Round the integer if m_mantissa > 10^numberOfSignificantDigits-1
  Integer m = unsignedMantissa();
  int numberOfDigitsInMantissa = Integer::NumberOfBase10DigitsWithoutSign(m);
  if (numberOfDigitsInMantissa > numberOfSignificantDigits) {
    IntegerDivision d = Integer::Division(m, Integer((int64_t)std::pow(10.0, numberOfDigitsInMantissa - numberOfSignificantDigits)));
    m = d.quotient;
    if (Integer::NaturalOrder(d.remainder, Integer((int64_t)(5.0*std::pow(10.0, numberOfDigitsInMantissa-numberOfSignificantDigits-1)))) >= 0) {
      m = Integer::Addition(m, Integer(1));
      // if 9999 was rounded to 10000, we need to update exponent and mantissa
      if (Integer::NumberOfBase10DigitsWithoutSign(m) > numberOfSignificantDigits) {
        exponent++;
        m = Integer::Division(m, Integer(10)).quotient;
      }
    }
    /* For example 1.999 with 3 significant digits: the mantissa 1999 is rounded
     * to 2000. To avoid printing 2.000, we removeZeroAtTheEnd here. */
    removeZeroAtTheEnd(&m);
  }
  if (m_negative) {
    buffer[currentChar++] = '-';
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
  }
  int mantissaLength = m.serialize(tempBuffer, PrintFloat::k_numberOfStoredSignificantDigits+1);
  assert(strcmp(tempBuffer, "inf") != 0 && strcmp(tempBuffer, "-inf") != 0);
  if (strcmp(tempBuffer, "undef") == 0) {
    currentChar += strlcpy(buffer+currentChar, tempBuffer, bufferSize-currentChar);
    return currentChar;
  }
  /* We force scientific mode if the number of digits before the dot is superior
   * to the number of significant digits (ie with 4 significant digits,
   * 12345 -> 1.235E4 or 12340 -> 1.234E4). */
  bool forceScientificMode = mode == Preferences::PrintFloatMode::Scientific || exponent >= numberOfSignificantDigits;
  int numberOfRequiredDigits = mantissaLength;
  if (!forceScientificMode) {
    numberOfRequiredDigits = mantissaLength > exponent ? mantissaLength : exponent;
    numberOfRequiredDigits = exponent < 0 ? mantissaLength-exponent : numberOfRequiredDigits;
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
    currentChar += Integer(exponent).serialize(buffer+currentChar, bufferSize-currentChar);
    return currentChar;
  }
  /* Case 1: Decimal mode */
  int deltaCharMantissa = exponent < 0 ? -exponent+1 : 0;
  strlcpy(buffer+currentChar+deltaCharMantissa, tempBuffer, max(0, bufferSize-deltaCharMantissa-currentChar));
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
    int decimalMarkerPosition = m_negative ? exponent + 1 : exponent;
    for (int i = currentChar-1; i > decimalMarkerPosition; i--) {
      buffer[i+1] = buffer[i];
    }
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
    buffer[decimalMarkerPosition+1] = '.';
    currentChar++;
  }
  if (exponent >= 0 && exponent > mantissaLength-1) {
    int endMarkerPosition = m_negative ? exponent+1 : exponent;
    for (int i = currentChar-1; i < endMarkerPosition; i++) {
      if (currentChar+1 >= bufferSize-1) { return bufferSize-1; }
      buffer[currentChar++] = '0';
    }
  }
  if (currentChar >= bufferSize-1) { return bufferSize-1; }
  buffer[currentChar] = 0;
  return currentChar;
}

template<typename T> T DecimalNode::templatedApproximate() const {
  Integer m = signedMantissa();
  T f = m.approximate<T>();
  int numberOfDigits = Integer::NumberOfBase10DigitsWithoutSign(m);
  T result = f*std::pow((T)10.0, (T)(m_exponent-numberOfDigits+1));
  return m_negative ? -result : result;
}

int Decimal::Exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength) {
  bool exponentNegative = false;
  if (exponentLength > 0 && exponent[0] == '-') {
    exponent++;
    exponentNegative = true;
    exponentLength--;
  }
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

Decimal::Decimal(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, int exponent) : Number() {
  /* Create a Decimal whose mantissa has less than
   * k_numberOfStoredSignificantDigits. We round exceeding number if necessary. */
  Integer zero(0);
  Integer base(10);
  // Get rid of useless preceeding 0s
  while (*integralPart == '0' && integralPartLength > 1) {
    integralPart++;
    integralPartLength--;
  }
  //TODO: set a FLAG to tell that a rounding happened?
  bool rounding = integralPartLength > PrintFloat::k_numberOfStoredSignificantDigits && integralPart[PrintFloat::k_numberOfStoredSignificantDigits] >= '5';
  // Cap the length of the integralPart
  integralPartLength = integralPartLength > PrintFloat::k_numberOfStoredSignificantDigits ? PrintFloat::k_numberOfStoredSignificantDigits : integralPartLength;
  Integer numerator(integralPart, integralPartLength, false);
  assert(!numerator.isInfinity());
  // Special case for 0.??? : get rid of useless 0s in front of the integralPartLength
  if (fractionalPart != nullptr && integralPartLength == 1 && integralPart[0] == '0') {
    integralPartLength = 0;
    while (*fractionalPart == '0') {
      fractionalPart++;
      fractionalPartLength--;
    }
  }
  rounding |= integralPartLength+fractionalPartLength > PrintFloat::k_numberOfStoredSignificantDigits && fractionalPart[PrintFloat::k_numberOfStoredSignificantDigits-integralPartLength] >= '5';
  fractionalPartLength = integralPartLength+fractionalPartLength > PrintFloat::k_numberOfStoredSignificantDigits ? PrintFloat::k_numberOfStoredSignificantDigits - integralPartLength : fractionalPartLength;
  for (int i = 0; i < fractionalPartLength; i++) {
    numerator = Integer::Multiplication(numerator, base);
    numerator = Integer::Addition(numerator, Integer(*fractionalPart-'0'));
    fractionalPart++;
  }
  numerator = rounding ? Integer::Addition(numerator, Integer(1)) : numerator;
  exponent = numerator.isZero() ? 0 : exponent;
  new (this) Decimal(numerator, exponent);
}

template <typename T>
Decimal::Decimal(T f) : Number() {
  assert(!std::isnan(f) && !std::isinf(f));
  int exp = IEEE754<T>::exponentBase10(f);
  /* We keep 7 significant digits for if the the Decimal was built from a float
   * and 14 significant digits if it was built from a double. This roughly
   * correspond to the respective precision of float and double. */
  int numberOfSignificantDigits = sizeof(T) == sizeof(float) ? PrintFloat::k_numberOfPrintedSignificantDigits : PrintFloat::k_numberOfStoredSignificantDigits;
  /* mantissa = f*10^(-exponent+numberOfSignificantDigits-1). We compute
   * this operations in 2 steps as
   * 10^(-exponent+numberOfSignificantDigits+1) can be infinity.*/
  double mantissaf = f * std::pow(10.0, (double)(-exp));
  mantissaf = mantissaf * std::pow((double)10.0, (double)(numberOfSignificantDigits-1));
  /* If m > 99999999999999.5 or 9999999,5, the mantissa stored will be 1 (as we keep only
   * 14 significative numbers from double. In that case, the exponent must be
   * increment as well. */
  static double biggestMantissaFromDouble = std::pow((double)10.0, (double)(numberOfSignificantDigits))-0.5;
  if (std::fabs(mantissaf) >= biggestMantissaFromDouble) {
    exp++;
  }
  Integer m = Integer((int64_t)(std::round(mantissaf)));
  /* We get rid of extra 0 at the end of the mantissa. */
  removeZeroAtTheEnd(&m);
  new (this) Decimal(m, exp);
}

/* We do not get rid of the useless 0s ending the mantissa here because we want
 * to keep them if they were entered by the user. */
Decimal::Decimal(Integer m, int e) :
  Decimal(sizeof(DecimalNode)+m.numberOfDigits()*sizeof(native_uint_t), m, e) {}


Decimal::Decimal(size_t size, const Integer & m, int e) : Number(TreePool::sharedPool()->createTreeNode<DecimalNode>(size)) {
  node()->setValue(m.digits(), m.numberOfDigits(), e, m.isNegative());
}

Expression Decimal::setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) {
  Decimal result = *this;
  result.node()->setNegative(s == ExpressionNode::Sign::Negative);
  return result;
}

Expression Decimal::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefined()) {
    return e;
  }
  // this = e
  int exp = node()->exponent();
  Integer numerator = node()->signedMantissa();
  /* To avoid uselessly big numerator and denominator, we get rid of useless 0s
   * ending the mantissa before reducing to Rational. */
  removeZeroAtTheEnd(&numerator);
  int numberOfDigits = Integer::NumberOfBase10DigitsWithoutSign(numerator);
  Integer denominator(1);
  if (exp >= numberOfDigits-1) {
    numerator = Integer::Multiplication(numerator, Integer::Power(Integer(10), Integer(exp-numberOfDigits+1)));
  } else {
    denominator = Integer::Power(Integer(10), Integer(numberOfDigits-1-exp));
  }
  Expression result;
  if (numerator.isInfinity() || denominator.isInfinity()) {
    result = Number::FloatNumber(node()->signedMantissa().template approximate<double>()*std::pow(10.0, (double)exp));
  } else {
    result = Rational(numerator, denominator);
  }
  replaceWithInPlace(result);
  return result;
}

Expression Decimal::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  if (sign() == ExpressionNode::Sign::Negative) {
    Expression abs = setSign(ExpressionNode::Sign::Positive, context, angleUnit);
    Opposite o;
    replaceWithInPlace(o);
    o.replaceChildAtIndexInPlace(0, abs);
    return o;
  }
  return *this;
}

template Decimal::Decimal(double);
template Decimal::Decimal(float);

}
