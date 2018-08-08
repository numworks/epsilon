#include <poincare/decimal.h>
#include <poincare/rational.h>
#include <poincare/opposite.h>
#include <poincare/infinity.h>
#include <poincare/layout_engine.h>
#include <poincare/ieee754.h>
#include <assert.h>
#include <ion.h>
#include <cmath>
#include <assert.h>

namespace Poincare {

void removeZeroAtTheEnd(IntegerReference * i) {
  if (i->isZero()) {
    return;
  }
  IntegerReference base = IntegerReference(10);
  IntegerDivisionReference d = IntegerReference::Division(*i, base);
  while (d.remainder.isZero()) {
    *i = d.quotient;
    d = IntegerReference::Division(*i, base);
  }
}

void DecimalNode::setValue(native_uint_t * mantissaDigits, size_t mantissaSize, int exponent, bool negative) {
  m_negative = negative;
  m_exponent = exponent;
  m_numberOfDigitsInMantissa = mantissaSize;
  memcpy(m_mantissa, mantissaDigits, mantissaSize*sizeof(native_uint_t));
}

NaturalIntegerPointer DecimalNode::mantissa() const {
  return NaturalIntegerPointer((native_uint_t *)m_mantissa, m_numberOfDigitsInMantissa);
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
    NaturalIntegerPointer m = mantissa();
    NaturalIntegerPointer otherM = other->mantissa();
    unsignedComparison = NaturalIntegerAbstract::ucmp(&m, &otherM);
  }
  return ((int)sign())*unsignedComparison;
}

Expression DecimalNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) {
  Expression e = ExpressionNode::shallowReduce(context, angleUnit);
  if (e.node() != this) {
    return e;
  }
  Expression reference(this);
  NaturalIntegerPointer m = mantissa();
  IntegerReference numerator(&m);
  removeZeroAtTheEnd(&numerator);
  int numberOfDigits = IntegerReference::NumberOfBase10Digits(numerator);
  IntegerReference denominator(1);
  if (m_exponent >= numberOfDigits-1) {
    numerator = IntegerReference::Multiplication(numerator, IntegerReference::Power(IntegerReference(10), IntegerReference(m_exponent-numberOfDigits+1)));
  } else {
    denominator = IntegerReference::Power(IntegerReference(10), IntegerReference(numberOfDigits-1-m_exponent));
  }
  // Do not reduce decimal to rational if the exponent is too big or too small.
  if (numerator.isInfinity()) {
    assert(!denominator.isInfinity());
    return InfinityReference(m_negative);
  }
  if (denominator.isInfinity()) {
    assert(!denominator.isInfinity());
    return RationalReference(0);
  }
  numerator.setNegative(m_negative);
  return RationalReference(numerator, denominator);
}

Expression DecimalNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  Expression reference(this);
  if (m_negative) {
    m_negative = false;
    return OppositeReference(reference);
  }
  return reference;
}

bool DecimalNode::needsParenthesisWithParent(SerializableNode * parentNode) const {
  if (!m_negative) {
    return false;
  }
  Type types[] = {Type::Addition, Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Power, Type::Factorial};
  return static_cast<ExpressionNode *>(parentNode)->isOfType(types, 7);
}

LayoutRef DecimalNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  char buffer[k_maxBufferSize];
  int numberOfChars = convertToText(buffer, k_maxBufferSize, floatDisplayMode, numberOfSignificantDigits);
  return LayoutEngine::createStringLayout(buffer, numberOfChars);
}

int DecimalNode::writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return convertToText(buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits);
}

int DecimalNode::convertToText(char * buffer, int bufferSize, Preferences::PrintFloatMode mode, int numberOfSignificantDigits) const {
  if (bufferSize == 0) {
    return -1;
  }
  buffer[bufferSize-1] = 0;
  int currentChar = 0;
  if (currentChar >= bufferSize-1) { return bufferSize-1; }
  if (mantissa().isZero()) {
    buffer[currentChar++] = '0';
    buffer[currentChar] = 0;
    return currentChar;
  }
  int exponent = m_exponent;
  char tempBuffer[PrintFloat::k_numberOfStoredSignificantDigits+1];
  // Round the integer if m_mantissa > 10^numberOfSignificantDigits-1
  NaturalIntegerPointer m = mantissa();
  IntegerReference mantissaRef(&m);
  int numberOfDigitsInMantissa = IntegerReference::NumberOfBase10Digits(mantissaRef);
  if (numberOfDigitsInMantissa > numberOfSignificantDigits) {
    IntegerDivisionReference d = IntegerReference::Division(mantissaRef, IntegerReference((int64_t)std::pow(10.0, numberOfDigitsInMantissa - numberOfSignificantDigits)));
    mantissaRef = d.quotient;
    if (IntegerReference::NaturalOrder(d.remainder, IntegerReference((int64_t)(5.0*std::pow(10.0, numberOfDigitsInMantissa-numberOfSignificantDigits-1)))) >= 0) {
      mantissaRef = IntegerReference::Addition(mantissaRef, IntegerReference(1));
      // if 9999 was rounded to 10000, we need to update exponent and mantissa
      if (IntegerReference::NumberOfBase10Digits(mantissaRef) > numberOfSignificantDigits) {
        exponent++;
        mantissaRef = IntegerReference::Division(mantissaRef, IntegerReference(10)).quotient;
      }
    }
    removeZeroAtTheEnd(&mantissaRef);
  }
  if (m_negative) {
    buffer[currentChar++] = '-';
    if (currentChar >= bufferSize-1) { return bufferSize-1; }
  }
  int mantissaLength = mantissaRef.writeTextInBuffer(tempBuffer, PrintFloat::k_numberOfStoredSignificantDigits+1, mode, numberOfSignificantDigits);
  if (strcmp(tempBuffer, "inf") == 0) {
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
    currentChar += IntegerReference(exponent).writeTextInBuffer(buffer+currentChar, bufferSize-currentChar, mode, numberOfSignificantDigits);
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

template<typename T> Evaluation<T> DecimalNode::templatedApproximate() const {
  NaturalIntegerPointer m = mantissa();
  T f = m.approximate<T>();
  int numberOfDigits = NaturalIntegerAbstract::NumberOfBase10Digits(&m);
  T result = f*std::pow((T)10.0, (T)(m_exponent-numberOfDigits+1));
  return Complex<T>(m_negative ? -result : result);
}

int DecimalReference::exponent(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, const char * exponent, int exponentLength, bool exponentNegative) {
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

DecimalReference::DecimalReference(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, bool negative, int exponent) {
  IntegerReference zero(0);
  IntegerReference base(10);
  IntegerReference numerator(integralPart, integralPartLength, negative);
  for (int i = 0; i < fractionalPartLength; i++) {
    numerator = IntegerReference::Multiplication(numerator, base);
    numerator = IntegerReference::Addition(numerator, IntegerReference(*fractionalPart-'0'));
    fractionalPart++;
  }
  *this = DecimalReference(numerator, exponent);
}

template <typename T>
DecimalReference::DecimalReference(T f) {
  assert(!std::isnan(f) && !std::isinf(f));
  int exp = IEEE754<T>::exponentBase10(f);
  int64_t mantissaf = std::round((double)f * std::pow((double)10.0, (double)(-exp+PrintFloat::k_numberOfStoredSignificantDigits+1)));
  IntegerReference m(mantissaf);
  *this= DecimalReference(IntegerReference(mantissaf), exp);
}

DecimalReference::DecimalReference(IntegerReference m, int e) {
  if (m.isAllocationFailure()) {
    *this = DecimalReference(ExpressionNode::FailedAllocationStaticNode());
    return;
  }
  *this = DecimalReference(sizeof(DecimalNode)+sizeof(native_uint_t)*m.numberOfDigits());
  if (!node()->isAllocationFailure()) {
    static_cast<DecimalNode *>(node())->setValue(m.typedNode()->digits(), m.typedNode()->numberOfDigits(), e, m.isNegative());
  }
}

template DecimalReference::DecimalReference(double);
template DecimalReference::DecimalReference(float);

}
