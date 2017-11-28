#include <poincare/complex.h>
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

namespace Poincare {


void PrintFloat::printBase10IntegerWithDecimalMarker(char * buffer, int bufferSize,  int i, int decimalMarkerPosition) {
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
      assert(endChar >= 0 && endChar < bufferSize);
      buffer[endChar--] = '.';
    }
    quotien = dividend/10;
    digit = dividend - quotien*10;
    assert(endChar >= 0 && endChar < bufferSize);
    buffer[endChar--] = '0'+digit;
    dividend = quotien;
  }  while (endChar >= startChar);
}

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
int Complex<T>::writeTextInBuffer(char * buffer, int bufferSize) const {
  return convertComplexToText(buffer, bufferSize, Preferences::sharedPreferences()->displayMode(), Preferences::sharedPreferences()->complexFormat(), Ion::Charset::MultiplicationSign);
}

template <class T>
int Complex<T>::convertFloatToText(T f, char * buffer, int bufferSize,
    int numberOfSignificantDigits, Expression::FloatDisplayMode mode) {
  assert(numberOfSignificantDigits > 0);
  if (mode == Expression::FloatDisplayMode::Default) {
    return convertFloatToText(f, buffer, bufferSize, numberOfSignificantDigits, Preferences::sharedPreferences()->displayMode());
  }
  char tempBuffer[k_maxFloatBufferLength];
  int requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits, mode);
  /* if the required buffer size overflows the buffer size, we first force the
   * display mode to scientific and decrease the number of significant digits to
   * fit the buffer size. If the buffer size is still to small, we only write
   * the beginning of the float and truncate it (which can result in a non sense
   * text) */
  if (mode == Expression::FloatDisplayMode::Decimal && requiredLength >= bufferSize) {
    requiredLength = convertFloatToTextPrivate(f, tempBuffer, numberOfSignificantDigits, Expression::FloatDisplayMode::Scientific);
  }
  if (requiredLength >= bufferSize) {
    int adjustedNumberOfSignificantDigits = numberOfSignificantDigits - requiredLength + bufferSize - 1;
    adjustedNumberOfSignificantDigits = adjustedNumberOfSignificantDigits < 1 ? 1 : adjustedNumberOfSignificantDigits;
    requiredLength = convertFloatToTextPrivate(f, tempBuffer, adjustedNumberOfSignificantDigits, Expression::FloatDisplayMode::Scientific);
  }
  requiredLength = requiredLength < bufferSize ? requiredLength : bufferSize;
  strlcpy(buffer, tempBuffer, bufferSize);
  return requiredLength;
}

template <class T>
Complex<T>::Complex(T a, T b) :
  m_a(a),
  m_b(b)
{
}

template <class T>
ExpressionLayout * Complex<T>::privateCreateLayout(Expression::FloatDisplayMode floatDisplayMode, Expression::ComplexFormat complexFormat) const {
  assert(floatDisplayMode != Expression::FloatDisplayMode::Default);
  if (complexFormat == Expression::ComplexFormat::Polar) {
    return createPolarLayout(floatDisplayMode);
  }
  return createCartesianLayout(floatDisplayMode);
}

template<typename T>
template<typename U>
Complex<U> * Complex<T>::templatedApproximate(Context& context, Expression::AngleUnit angleUnit) const {
  return new Complex<U>(Complex<U>::Cartesian((U)m_a, (U)m_b));
}

template <class T>
int Complex<T>::convertComplexToText(char * buffer, int bufferSize, Expression::FloatDisplayMode displayMode, Expression::ComplexFormat complexFormat, char multiplicationSpecialChar) const {
  assert(displayMode != Expression::FloatDisplayMode::Default);
  int numberOfChars = 0;
  if (std::isnan(m_a) || std::isnan(m_b)) {
    return convertFloatToText(NAN, buffer, bufferSize, k_numberOfSignificantDigits, displayMode);
  }
  if (complexFormat == Expression::ComplexFormat::Polar) {
    if (r() != 1 || th() == 0) {
      numberOfChars = convertFloatToText(r(), buffer, bufferSize, k_numberOfSignificantDigits, displayMode);
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
      numberOfChars += convertFloatToText(th(), buffer+numberOfChars, bufferSize-numberOfChars, k_numberOfSignificantDigits, displayMode);
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
    numberOfChars = convertFloatToText(m_a, buffer, bufferSize, k_numberOfSignificantDigits, displayMode);
    if (m_b > 0 && !std::isnan(m_b) && bufferSize > numberOfChars+1) {
      buffer[numberOfChars++] = '+';
      // Ensure that the string is null terminated even if buffer size is to small
      buffer[numberOfChars] = 0;
    }
  }
  if (m_b != 1 && m_b != -1 && m_b != 0) {
    numberOfChars += convertFloatToText(m_b, buffer+numberOfChars, bufferSize-numberOfChars, k_numberOfSignificantDigits, displayMode);
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
int Complex<T>::convertFloatToTextPrivate(T f, char * buffer, int numberOfSignificantDigits, Expression::FloatDisplayMode mode) {
  assert(mode != Expression::FloatDisplayMode::Default);
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

  T logBase10 = f != 0 ? std::log10(std::fabs(f)) : 0;
  int exponentInBase10 = std::floor(logBase10);
  /* Correct the exponent in base 10: sometines the exact log10 of f is 6.999999
   * but is stored as 7 in hardware. We catch these cases here. */
  if (f != 0 && logBase10 == (int)logBase10 && std::fabs(f) < std::pow(10, logBase10)) {
    exponentInBase10--;
  }

  Expression::FloatDisplayMode displayMode = mode;
  if ((exponentInBase10 >= numberOfSignificantDigits || exponentInBase10 <= -numberOfSignificantDigits) && mode == Expression::FloatDisplayMode::Decimal) {
    displayMode = Expression::FloatDisplayMode::Scientific;
  }

  // Number of char available for the mantissa
  int availableCharsForMantissaWithoutSign = numberOfSignificantDigits + 1;
  int availableCharsForMantissaWithSign = f >= 0 ? availableCharsForMantissaWithoutSign : availableCharsForMantissaWithoutSign + 1;

  // Compute mantissa
  /* The number of digits in an integer is capped because the maximal integer is
   * 2^31 - 1. As our mantissa is an integer, we assert that we stay beyond this
   * threshold during computation. */
  assert(availableCharsForMantissaWithoutSign - 1 < std::log10(std::pow(2.0f, 31.0f)));

  int numberOfDigitBeforeDecimal = exponentInBase10 >= 0 || displayMode == Expression::FloatDisplayMode::Scientific ?
                                   exponentInBase10 + 1 : 1;
  T mantissa = std::round(f * std::pow(10, (T)availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal));
  /* if availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal
   * is too big (or too small), mantissa is now inf. We handle this case by
   * using logarithm function. */
  if (std::isnan(mantissa) || std::isinf(mantissa)) {
    mantissa = std::round(std::pow(10, std::log10(std::fabs(f))+(T)(availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal)));
    mantissa = std::copysign(mantissa, f);
  }
  /* We update the exponent in base 10 (if 0.99999999 was rounded to 1 for
   * instance) */
  T truncatedMantissa = (int)(f * std::pow(10, (T)(availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal)));
  if (std::isinf(truncatedMantissa) || std::isnan(truncatedMantissa)) {
    truncatedMantissa = (int)(std::pow(10, std::log10(std::fabs(f))+(T)(availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal)));
    truncatedMantissa = std::copysign(truncatedMantissa, f);
  }
  if (mantissa != truncatedMantissa) {
    T newLogBase10 = mantissa != 0 ? std::log10(std::fabs(mantissa/std::pow((T)10, (T)(availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal)))) : 0;
    if (std::isnan(newLogBase10) || std::isinf(newLogBase10)) {
      newLogBase10 = std::log10(std::fabs((T)mantissa)) - (T)(availableCharsForMantissaWithoutSign - 1 - numberOfDigitBeforeDecimal);
    }
    exponentInBase10 = std::floor(newLogBase10);
  }
  int decimalMarkerPosition = exponentInBase10 < 0 || displayMode == Expression::FloatDisplayMode::Scientific ?
    1 : exponentInBase10+1;

  // Correct the number of digits in mantissa after rounding
  int mantissaExponentInBase10 = exponentInBase10 > 0 || displayMode == Expression::FloatDisplayMode::Scientific ? availableCharsForMantissaWithoutSign - 1 : availableCharsForMantissaWithoutSign + exponentInBase10;
  if ((int)(std::fabs(mantissa) * std::pow((T)10, - mantissaExponentInBase10)) > 0) {
    mantissa = mantissa/10;
  }

  int numberOfCharExponent = exponentInBase10 != 0 ? std::log10(std::fabs((T)exponentInBase10)) + 1 : 1;
  if (exponentInBase10 < 0){
    // If the exponent is < 0, we need a additional char for the sign
    numberOfCharExponent++;
  }

  // Supress the 0 on the right side of the mantissa
  int dividend = std::fabs((T)mantissa);
  int quotien = dividend/10;
  int digit = dividend - quotien*10;
  int minimumNumberOfCharsInMantissa = 1;
  while (digit == 0 && availableCharsForMantissaWithoutSign > minimumNumberOfCharsInMantissa &&
      (availableCharsForMantissaWithoutSign > exponentInBase10+2 || displayMode == Expression::FloatDisplayMode::Scientific)) {
    mantissa = mantissa/10;
    availableCharsForMantissaWithoutSign--;
    availableCharsForMantissaWithSign--;
    dividend = quotien;
    quotien = dividend/10;
    digit = dividend - quotien*10;
  }

  // Suppress the decimal marker if no fractional part
  if ((displayMode == Expression::FloatDisplayMode::Decimal && availableCharsForMantissaWithoutSign == exponentInBase10+2)
      || (displayMode == Expression::FloatDisplayMode::Scientific && availableCharsForMantissaWithoutSign == 2)) {
    availableCharsForMantissaWithSign--;
  }

  // Print mantissa
  assert(availableCharsForMantissaWithSign < k_maxFloatBufferLength);
  PrintFloat::printBase10IntegerWithDecimalMarker(buffer, availableCharsForMantissaWithSign, mantissa, decimalMarkerPosition);
  if (displayMode == Expression::FloatDisplayMode::Decimal || exponentInBase10 == 0) {
    buffer[availableCharsForMantissaWithSign] = 0;
    return availableCharsForMantissaWithSign;
  }
  // Print exponent
  assert(availableCharsForMantissaWithSign < k_maxFloatBufferLength);
  buffer[availableCharsForMantissaWithSign] = Ion::Charset::Exponent;
  assert(numberOfCharExponent+availableCharsForMantissaWithSign+1 < k_maxFloatBufferLength);
  PrintFloat::printBase10IntegerWithDecimalMarker(buffer+availableCharsForMantissaWithSign+1, numberOfCharExponent, exponentInBase10, -1);
  buffer[availableCharsForMantissaWithSign+1+numberOfCharExponent] = 0;
  return (availableCharsForMantissaWithSign+1+numberOfCharExponent);
}

template <class T>
ExpressionLayout * Complex<T>::createPolarLayout(Expression::FloatDisplayMode floatDisplayMode) const {
  char bufferBase[k_maxFloatBufferLength+2];
  int numberOfCharInBase = 0;
  char bufferSuperscript[k_maxFloatBufferLength+2];
  int numberOfCharInSuperscript = 0;

  if (std::isnan(r()) || (std::isnan(th()) && r() != 0)) {
    numberOfCharInBase = convertFloatToText(NAN, bufferBase, k_maxComplexBufferLength, k_numberOfSignificantDigits, floatDisplayMode);
    return new StringLayout(bufferBase, numberOfCharInBase);
  }
  if (r() != 1 || th() == 0) {
    numberOfCharInBase = convertFloatToText(r(), bufferBase, k_maxFloatBufferLength, k_numberOfSignificantDigits, floatDisplayMode);
    if (r() != 0 && th() != 0) {
      bufferBase[numberOfCharInBase++] = Ion::Charset::MiddleDot;
    }
  }
  if (r() != 0 && th() != 0) {
    bufferBase[numberOfCharInBase++] = Ion::Charset::Exponential;
    bufferBase[numberOfCharInBase] = 0;
  }

  if (r() != 0 && th() != 0) {
    numberOfCharInSuperscript = convertFloatToText(th(), bufferSuperscript, k_maxFloatBufferLength, k_numberOfSignificantDigits, floatDisplayMode);
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
ExpressionLayout * Complex<T>::createCartesianLayout(Expression::FloatDisplayMode floatDisplayMode) const {
  char buffer[k_maxComplexBufferLength];
  int numberOfChars = convertComplexToText(buffer, k_maxComplexBufferLength, floatDisplayMode, Expression::ComplexFormat::Cartesian, Ion::Charset::MiddleDot);
  return new StringLayout(buffer, numberOfChars);
}

template class Complex<float>;
template class Complex<double>;
template Complex<double>* Complex<double>::templatedApproximate<double>(Context&, Expression::AngleUnit) const;
template Complex<float>* Complex<double>::templatedApproximate<float>(Context&, Expression::AngleUnit) const;
template Complex<double>* Complex<float>::templatedApproximate<double>(Context&, Expression::AngleUnit) const;
template Complex<float>* Complex<float>::templatedApproximate<float>(Context&, Expression::AngleUnit) const;

}

