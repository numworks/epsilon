#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <stdint.h>
#include <assert.h>
#include <poincare/expression_layout.h>

namespace Poincare {

/* All algorithm should be improved with:
 * Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann */

struct IntegerDivision;

class Integer {
public:
  typedef uint16_t half_native_uint_t;
  typedef int32_t native_int_t;
  typedef int64_t double_native_int_t;
  typedef uint32_t native_uint_t;
  typedef uint64_t double_native_uint_t;

  // FIXME: This constructor should be constexpr
  Integer(native_int_t i = 0) :
    m_digit(i>0 ? i : -i),
    m_numberOfDigits(1),
    m_negative(i<0)
  {
  }
  Integer(double_native_int_t i);
  Integer(const char * digits, bool negative = false); // Digits are NOT NULL-terminated
  static Integer exponent(int fractionalPartLength, const char * exponent, int exponentLength, bool exponentNegative);
  static Integer numerator(const char * integralPart, int integralPartLength, const char * fractionalPart, int fractionalPartLength, bool negative, Integer * exponent);
  static Integer denominator(Integer * exponent);

  ~Integer();
  Integer(Integer&& other); // C++11 move constructor
  Integer& operator=(Integer&& other); // C++11 move assignment operator
  Integer(const Integer& other); // C++11 copy constructor
  Integer& operator=(const Integer& other); // C++11 copy assignment operator

  // Getter & Setter
  bool isNegative() const { return m_negative; }
  void setNegative(bool negative);
  int extractedInt() const { assert(m_numberOfDigits == 1 && m_digit <= k_maxExtractableInteger); return m_negative ? -m_digit : m_digit; }

  // Comparison
  static int NaturalOrder(const Integer & i, const Integer & j);
  bool isEqualTo(const Integer & other) const;
  bool isLowerThan(const Integer & other) const;

  // Layout
  int writeTextInBuffer(char * buffer, int bufferSize) const;
  ExpressionLayout * createLayout() const;

  // Approximation
  template<typename T> T approximate() const;

  // Arithmetic
  static Integer Addition(const Integer & i, const Integer & j);
  static Integer Subtraction(const Integer & i, const Integer & j);
  static Integer Multiplication(const Integer & i, const Integer & j);
  static Integer Factorial(const Integer & i);
  static IntegerDivision Division(const Integer & numerator, const Integer & denominator);
  static Integer Power(const Integer & i, const Integer & j);
  //static Integer Division(const Integer & i, const Integer & j);
  //static IntegerDivision division(const Integer & i, const Integer & j);
  bool isOne() const { return (m_numberOfDigits == 1 && digit(0) == 1 && !m_negative); };
  bool isTwo() const { return (m_numberOfDigits == 1 && digit(0) == 2 && !m_negative); };
  bool isTen() const { return (m_numberOfDigits == 1 && digit(0) == 10 && !m_negative); };
  bool isMinusOne() const { return (m_numberOfDigits == 1 && digit(0) == 1 && m_negative); };
  bool isZero() const { return (m_numberOfDigits == 1 && digit(0) == 0); };
  constexpr static int k_maxExtractableInteger = 0x7FFFFFFF;
private:
  Integer(const native_uint_t * digits, uint16_t numberOfDigits, bool negative);
  static Integer IntegerWithHalfDigitAtIndex(half_native_uint_t halfDigit, int index);

  void releaseDynamicIvars();
  static int8_t ucmp(const Integer & a, const Integer & b); // -1, 0, or 1
  static Integer usum(const Integer & a, const Integer & b, bool subtract, bool outputNegative);
  static Integer addition(const Integer & a, const Integer & b, bool inverseBNegative);
  static IntegerDivision udiv(const Integer & a, const Integer & b);
  bool usesImmediateDigit() const { return m_numberOfDigits == 1; }
  native_uint_t digit(int i) const {
    assert(i >= 0 && i < m_numberOfDigits);
    return (usesImmediateDigit() ? m_digit : m_digits[i]);
  }
  uint16_t numberOfHalfDigits() const {
    native_uint_t d = digit(m_numberOfDigits-1);
    native_uint_t halfBase = 1 << (8*sizeof(half_native_uint_t));
    return (d >= halfBase ? 2*m_numberOfDigits : 2*m_numberOfDigits-1);
  }
  half_native_uint_t halfDigit(int i) const {
    assert(i >= 0);
    if (i >= numberOfHalfDigits()) {
      return 0;
    }
    return (usesImmediateDigit() ? ((half_native_uint_t *)&m_digit)[i] : ((half_native_uint_t *)m_digits)[i]);
  }
  // Small integer optimization. Similar to short string optimization.
  union {
    const native_uint_t * m_digits; // Little-endian
    native_uint_t m_digit;
  };
  uint16_t m_numberOfDigits; // In base native_uint_max
  bool m_negative; // Make sure zero cannot be negative

  static_assert(sizeof(native_int_t) <= sizeof(native_uint_t), "native_uint_t should be able to contain native_int_t data");
  static_assert(sizeof(double_native_uint_t) == 2*sizeof(native_uint_t), "double_native_uint_t should be twice the size of native_uint_t");
};

struct IntegerDivision {
  Integer quotient;
  Integer remainder;
};

}

#endif
