#include <poincare/complex.h>
#include <poincare/undefined.h>
#include <poincare/decimal.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/symbol.h>
#include <poincare/ieee754.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
}
#include <cmath>
#include "layout/string_layout.h"
#include "layout/baseline_relative_layout.h"
#include <ion.h>
#include <stdio.h>

namespace Poincare {

template<typename T>
Complex<T> Complex<T>::Float(T x) {
  return Complex(x,0);
}

template<typename T>
Complex<T> Complex<T>::Cartesian(T a, T b) {
  return Complex(a,b);
}

template<typename T>
Complex<T> Complex<T>::Polar(T r, T th)  {
  // If the radius is 0, theta may be undefined but shouldn't be able to affect the result.
  if (r == 0) {
    return Complex(0,0);
  }
  T c = std::cos(th);
  T s = std::sin(th);
  /* Cheat: see comment on cosine.cpp.
   * Sine and cosine openbsd immplementationd are numerical approximation.
   * We though want to avoid evaluating e^(i*pi) to -1+1E-17i. We thus round
   * cosine and sine results to 0 if they are negligible compared to the
   * argument th. */
  c = th != 0 && std::fabs(c/th) <= Expression::epsilon<T>() ? 0 : c;
  s = th != 0 && std::fabs(s/th) <= Expression::epsilon<T>() ? 0 : s;
  return Complex(r*c,r*s);
}

template<typename T>
Complex<T>::Complex(const Complex<T> & other) {
  m_a = other.m_a;
  m_b = other.m_b;
}

template<typename T>
Complex<T> & Complex<T>::operator=(const Complex& other) {
  m_a = other.m_a;
  m_b = other.m_b;
  return *this;
}

template<typename T>
static inline T privateFloatSetSign(T f, bool negative) {
  if (negative) {
    return -f;
  }
  return f;
}

template<typename T>
T digitsToFloat(const char * digits, int length) {
  if (digits == nullptr) {
    return 0;
  }
  T result = 0;
  const char * digit = digits;
  for (int i = 0; i < length; i++) {
    result = 10 * result;
    result += *digit-'0';
    digit++;
  }
  return result;
}

template<typename T>
Complex<T>::Complex(const char * integralPart, int integralPartLength, bool integralNegative,
    const char * fractionalPart, int fractionalPartLength,
    const char * exponent, int exponentLength, bool exponentNegative) {
  T i = digitsToFloat<T>(integralPart, integralPartLength);
  T j = digitsToFloat<T>(fractionalPart, fractionalPartLength);
  T l = privateFloatSetSign<T>(digitsToFloat<T>(exponent, exponentLength), exponentNegative);

  m_a = privateFloatSetSign((i + j*std::pow(10, -std::ceil((T)fractionalPartLength)))* std::pow(10, l), integralNegative);
  m_b = 0;
}

template <class T>
T Complex<T>::a() const {
  return m_a;
}

template <class T>
T Complex<T>::b() const {
  return m_b;
}

template <class T>
T Complex<T>::r() const {
  // We want to avoid a^2 and b^2 which could both easily overflow.
  // min, max = minmax(abs(a), abs(b)) (*minmax returns both arguments sorted*)
  // abs(a + bi) == sqrt(a^2 + b^2)
  //             == sqrt(abs(a)^2 + abs(b)^2)
  //             == sqrt(min^2 + max^2)
  //             == sqrt((min^2 + max^2) * max^2/max^2)
  //             == sqrt((min^2 + max^2) / max^2)*sqrt(max^2)
  //             == sqrt(min^2/max^2 + 1) * max
  //             == sqrt((min/max)^2 + 1) * max
  // min >= 0 &&
  // max >= 0 &&
  // min <= max => min/max <= 1
  //            => (min/max)^2 <= 1
  //            => (min/max)^2 + 1 <= 2
  //            => sqrt((min/max)^2 + 1) <= sqrt(2)
  // So the calculation is guaranteed to not overflow until the final multiply.
  // If (min/max)^2 underflows then min doesn't contribute anything significant
  // compared to max, and the formula reduces to simply max as it should.
  // We do need to be careful about the case where a == 0 && b == 0 which would
  // cause a division by zero.
  T min = std::fabs(m_a);
  if (m_b == 0) {
    return min;
  }
  T max = std::fabs(m_b);
  if (max < min) {
    T temp = min;
    min = max;
    max = temp;
  }
  T temp = min/max;
  return std::sqrt(temp*temp + 1) * max;
}

template <class T>
T Complex<T>::th() const {
  T result = std::atan(m_b/m_a) + M_PI;
  if (m_a >= 0) {
    T a = m_a == 0 ? 0 : m_a;
    result = std::atan(m_b/a);
  }
  if (result > M_PI + FLT_EPSILON) {
    result = result - 2*M_PI;
  }
  return result;
}

template <class T>
Complex<T> Complex<T>::conjugate() const {
  return Cartesian(m_a, -m_b);
}

template<typename T>
Expression::Type Complex<T>::type() const {
  return Expression::Type::Complex;
}

template <class T>
Complex<T> * Complex<T>::clone() const {
  return new Complex<T>(*this);
}

template<typename T>
T Complex<T>::toScalar() const {
  if (m_b != 0) {
    return NAN;
  }
  return m_a;
}

template <class T>
int Complex<T>::writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits) const {
  return convertComplexToText(buffer, bufferSize, numberOfSignificantDigits, Preferences::sharedPreferences()->displayMode(), Preferences::sharedPreferences()->complexFormat(), Ion::Charset::MultiplicationSign);
}

template <class T>
bool Complex<T>::needParenthesisWithParent(const Expression * e) const {
  switch (e->type()) {
    case Type::Addition:
      return m_a < 0.0 || (m_a == 0.0 && m_b < 0.0);
    case Type::Subtraction:
    case Type::Multiplication:
    case Type::Opposite:
      return m_a < 0.0 || m_b < 0.0 || (m_a != 0.0 && m_b != 0.0);
    case Type::Factorial:
    case Type::Power:
    case Type::Division:
      return m_a < 0.0 || m_b != 0.0;
    default:
      return false;
  }
}



template <class T>
Complex<T>::Complex(T a, T b) :
  m_a(a),
  m_b(b)
{
}

template <class T>
ExpressionLayout * Complex<T>::privateCreateLayout(PrintFloat::Mode floatDisplayMode, Expression::ComplexFormat complexFormat) const {
  assert(floatDisplayMode != PrintFloat::Mode::Default);
  if (complexFormat == Expression::ComplexFormat::Polar) {
    return createPolarLayout(floatDisplayMode);
  }
  return createCartesianLayout(floatDisplayMode);
}

template <class T>
Expression * Complex<T>::CreateDecimal(T f) {
  if (std::isnan(f) || std::isinf(f)) {
    return new Undefined();
  }
  int e = IEEE754<T>::exponentBase10(f);
  int64_t mantissaf = f * std::pow((T)10, -e+PrintFloat::k_numberOfStoredSignificantDigits+1);
  return new Decimal(Integer(mantissaf), e);
}

template <class T>
Expression * Complex<T>::shallowReduce(Context & context, AngleUnit angleUnit) {
  Expression * a = CreateDecimal(m_a);
  Expression * b = CreateDecimal(m_b);
  Multiplication * m = new Multiplication(new Symbol(Ion::Charset::IComplex), b, false);
  Addition * add = new Addition(a, m, false);
  a->shallowReduce(context, angleUnit);
  b->shallowReduce(context, angleUnit);
  m->shallowReduce(context, angleUnit);
  return replaceWith(add, true)->shallowReduce(context, angleUnit);
}

template<typename T>
template<typename U>
Complex<U> * Complex<T>::templatedApproximate(Context& context, Expression::AngleUnit angleUnit) const {
  return new Complex<U>(Complex<U>::Cartesian((U)m_a, (U)m_b));
}

template <class T>
int Complex<T>::convertComplexToText(char * buffer, int bufferSize, int numberOfSignificantDigits, PrintFloat::Mode displayMode, Expression::ComplexFormat complexFormat, char multiplicationSpecialChar) const {
  assert(displayMode != PrintFloat::Mode::Default);
  int numberOfChars = 0;
  if (std::isnan(m_a) || std::isnan(m_b) || std::isinf(m_a) || std::isinf(m_b)) {
    return PrintFloat::convertFloatToText<T>(NAN, buffer, bufferSize, numberOfSignificantDigits, displayMode);
  }
  if (complexFormat == Expression::ComplexFormat::Polar) {
    if (r() != 1 || th() == 0) {
      numberOfChars = PrintFloat::convertFloatToText<T>(r(), buffer, bufferSize, numberOfSignificantDigits, displayMode);
      if (r() != 0 && th() != 0 && bufferSize > numberOfChars+1) {
        buffer[numberOfChars++] = multiplicationSpecialChar;
        // Ensure that the string is null terminated even if buffer size is to small
        buffer[numberOfChars] = 0;
      }
    }
    if (r() != 0 && th() != 0) {
      if (bufferSize > numberOfChars+3) {
        buffer[numberOfChars++] = Ion::Charset::Exponential;
        buffer[numberOfChars++] = '^';
        buffer[numberOfChars++] = '(';
        // Ensure that the string is null terminated even if buffer size is to small
        buffer[numberOfChars] = 0;
      }
      numberOfChars += PrintFloat::convertFloatToText<T>(th(), buffer+numberOfChars, bufferSize-numberOfChars, numberOfSignificantDigits, displayMode);
      if (bufferSize > numberOfChars+3) {
        buffer[numberOfChars++] = multiplicationSpecialChar;
        buffer[numberOfChars++] = Ion::Charset::IComplex;
        buffer[numberOfChars++] = ')';
        buffer[numberOfChars] = 0;
      }
    }
    return numberOfChars;
  }

  if (m_a != 0 || m_b == 0) {
    numberOfChars = PrintFloat::convertFloatToText<T>(m_a, buffer, bufferSize, numberOfSignificantDigits, displayMode);
    if (m_b > 0 && !std::isnan(m_b) && bufferSize > numberOfChars+1) {
      buffer[numberOfChars++] = '+';
      // Ensure that the string is null terminated even if buffer size is to small
      buffer[numberOfChars] = 0;
    }
  }
  if (m_b != 1 && m_b != -1 && m_b != 0) {
    numberOfChars += PrintFloat::convertFloatToText<T>(m_b, buffer+numberOfChars, bufferSize-numberOfChars, numberOfSignificantDigits, displayMode);
    buffer[numberOfChars++] = multiplicationSpecialChar;
  }
  if (m_b == -1 && bufferSize > numberOfChars+1) {
    buffer[numberOfChars++] = '-';
  }
  if (m_b != 0 && bufferSize > numberOfChars+1) {
    buffer[numberOfChars++] = Ion::Charset::IComplex;
    buffer[numberOfChars] = 0;
  }
  return numberOfChars;
}

template <class T>
ExpressionLayout * Complex<T>::createPolarLayout(PrintFloat::Mode floatDisplayMode) const {
  char bufferBase[PrintFloat::k_maxFloatBufferLength+2];
  int numberOfCharInBase = 0;
  char bufferSuperscript[PrintFloat::k_maxFloatBufferLength+2];
  int numberOfCharInSuperscript = 0;

  if (std::isnan(r()) || (std::isnan(th()) && r() != 0)) {
    numberOfCharInBase = PrintFloat::convertFloatToText<T>(NAN, bufferBase, PrintFloat::k_maxComplexBufferLength, Preferences::sharedPreferences()->numberOfSignificantDigits(), floatDisplayMode);
    return new StringLayout(bufferBase, numberOfCharInBase);
  }
  if (r() != 1 || th() == 0) {
    numberOfCharInBase = PrintFloat::convertFloatToText<T>(r(), bufferBase, PrintFloat::k_maxFloatBufferLength, Preferences::sharedPreferences()->numberOfSignificantDigits(), floatDisplayMode);
    if (r() != 0 && th() != 0) {
      bufferBase[numberOfCharInBase++] = Ion::Charset::MiddleDot;
    }
  }
  if (r() != 0 && th() != 0) {
    bufferBase[numberOfCharInBase++] = Ion::Charset::Exponential;
    bufferBase[numberOfCharInBase] = 0;
  }

  if (r() != 0 && th() != 0) {
    numberOfCharInSuperscript = PrintFloat::convertFloatToText<T>(th(), bufferSuperscript, PrintFloat::k_maxFloatBufferLength, Preferences::sharedPreferences()->numberOfSignificantDigits(), floatDisplayMode);
    bufferSuperscript[numberOfCharInSuperscript++] = Ion::Charset::MiddleDot;
    bufferSuperscript[numberOfCharInSuperscript++] = Ion::Charset::IComplex;
    bufferSuperscript[numberOfCharInSuperscript] = 0;
  }
  if (numberOfCharInSuperscript == 0) {
    return new StringLayout(bufferBase, numberOfCharInBase);
  }
  return new BaselineRelativeLayout(new StringLayout(bufferBase, numberOfCharInBase), new StringLayout(bufferSuperscript, numberOfCharInSuperscript), BaselineRelativeLayout::Type::Superscript);
}

template <class T>
ExpressionLayout * Complex<T>::createCartesianLayout(PrintFloat::Mode floatDisplayMode) const {
  char buffer[PrintFloat::k_maxComplexBufferLength];
  int numberOfChars = convertComplexToText(buffer, PrintFloat::k_maxComplexBufferLength, Preferences::sharedPreferences()->numberOfSignificantDigits(), floatDisplayMode, Expression::ComplexFormat::Cartesian, Ion::Charset::MiddleDot);
  return new StringLayout(buffer, numberOfChars);
}

template class Complex<float>;
template class Complex<double>;
template Complex<double>* Complex<double>::templatedApproximate<double>(Context&, Expression::AngleUnit) const;
template Complex<float>* Complex<double>::templatedApproximate<float>(Context&, Expression::AngleUnit) const;
template Complex<double>* Complex<float>::templatedApproximate<double>(Context&, Expression::AngleUnit) const;
template Complex<float>* Complex<float>::templatedApproximate<float>(Context&, Expression::AngleUnit) const;

}

