#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <stdint.h>
#include <assert.h>
#include <poincare/horizontal_layout.h>

namespace Poincare {

class ExpressionLayout;
class Layout;
class LayoutNode;
class Integer;
struct IntegerDivision;

typedef uint16_t half_native_uint_t;
typedef int32_t native_int_t;
typedef int64_t double_native_int_t;
typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

static_assert(sizeof(double_native_int_t) <= sizeof(double_native_uint_t), "double_native_int_t type has not the right size compared to double_native_uint_t");
static_assert(sizeof(native_int_t) == sizeof(native_uint_t), "native_int_t type has not the right size compared to native_uint_t");
static_assert(sizeof(double_native_uint_t) == 2*sizeof(native_uint_t), "double_native_uint_t should be twice the size of native_uint_t");
static_assert(sizeof(double_native_int_t) == 2*sizeof(native_int_t), "double_native_int_t type has not the right size compared to native_int_t");

/* All algorithm should be improved with:
 * Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann */

struct IntegerDivision;

class Integer final {
public:
  /* Constructors & Destructors */
  Integer(native_int_t i = 0);
  Integer(double_native_int_t i);
  Integer(const char * digits, size_t length, bool negative);
  Integer(const char * digits) : Integer(digits, strlen(digits), false) {}
  static Integer Overflow(bool negative) { return Integer((native_uint_t *)nullptr, k_maxNumberOfDigits+1, negative); }
  static Integer BuildInteger(native_uint_t * digits, uint16_t numberOfDigits, bool negative, bool enableOverflow = false);
  ~Integer();
  static void TidyIntegerBuffer();


#if POINCARE_TREE_LOG
  void log(std::ostream & stream = std::cout) const;
#endif

  /* Copy/Move constructors/assignments */
  Integer(Integer&& other); // C++11 move constructor
  Integer& operator=(Integer&& other); // C++11 move assignment operator
  Integer(const Integer& other); // C++11 copy constructor
  Integer& operator=(const Integer& other); // C++11 copy assignment operator

  // Getters
  const native_uint_t * digits() const { return usesImmediateDigit() ? &m_digit : m_digits; }
  uint8_t numberOfDigits() const { return m_numberOfDigits; }

  // Serialization
  int serialize(char * buffer, int bufferSize) const;

  //Layout
  HorizontalLayout createLayout() const;

  // Approximation
  template<typename T> T approximate() const;

  // Sign
  bool isNegative() const { return m_negative; }
  void setNegative(bool negative) { m_negative = m_numberOfDigits > 0 ? negative : false; }

  // Properties
  static int NumberOfBase10DigitsWithoutSign(const Integer & i);
  bool isOne() const { return (m_numberOfDigits == 1 && digit(0) == 1 && !m_negative); };
  bool isTwo() const { return (m_numberOfDigits == 1 && digit(0) == 2 && !m_negative); };
  bool isTen() const { return (m_numberOfDigits == 1 && digit(0) == 10 && !m_negative); };
  bool isMinusOne() const { return (m_numberOfDigits == 1 && digit(0) == 1 && m_negative); };
  bool isZero() const { return (m_numberOfDigits == 0); };
  bool isInfinity() const { return m_numberOfDigits > k_maxNumberOfDigits; }
  bool isEven() const { return ((digit(0) & 1) == 0); }

  constexpr static int k_maxExtractableInteger = 0x7FFFFFFF;
  int extractedInt() const { assert(m_numberOfDigits == 0 || (m_numberOfDigits <= 1 && digit(0) <= k_maxExtractableInteger)); return m_numberOfDigits == 0 ? 0 : (m_negative ? -digit(0) : digit(0)); }

  // Comparison
  static int NaturalOrder(const Integer & i, const Integer & j);
  bool isEqualTo(const Integer & other) const {
    return (NaturalOrder(*this, other) == 0);
  }
  bool isLowerThan(const Integer & other) const {
    return (NaturalOrder(*this, other) < 0);
  }

    // Arithmetic
  static Integer Addition(const Integer & i, const Integer & j) { return addition(i, j, false); }
  static Integer Subtraction(const Integer & i, const Integer & j) { return addition(i, j, true); }
  static Integer Multiplication(const Integer & i, const Integer & j) { return multiplication(i, j); }
  static IntegerDivision Division(const Integer & numerator, const Integer & denominator);
  static Integer Power(const Integer & i, const Integer & j);
  static Integer Factorial(const Integer & i);

  constexpr static int k_maxNumberOfDigits = 32;
  constexpr static int k_maxNumberOfIntegerSimutaneously = 16;
private:
  constexpr static int k_maxNumberOfDigitsBase10 = 308; // (2^32)^k_maxNumberOfDigits ~ 1E308

  Integer(native_uint_t * digits, uint16_t numberOfDigits, bool negative, bool enableOverflow = false);

  // Dynamic allocation
  /* In order to guarantee the potential existence of 16 Integers simutaneously,
   * we keep a table uint32_t that can contain up to 16 Integers with the
   * maximal numbers of digits. We also give them one extra digit to be able to
   * perform complex operations (like division) which involve Integers with one
   *  additional digit. */
  static native_uint_t * allocDigits(int numberOfDigits);
  static void freeDigits(native_uint_t * digits);

  // Constructors
  void releaseDynamicIvars();

  // Arithmetic
  static Integer addition(const Integer & a, const Integer & b, bool inverseBNegative, bool enableOneDigitOverflow = false);
  static Integer multiplication(const Integer & a, const Integer & b, bool enableOneDigitOverflow = false);

  /* buffer has to be k_maxNumberOfDigits+1 to allow temporary overflow (ie, in
   * subtraction) */
  static int8_t ucmp(const Integer & a, const Integer & b); // -1, 0, or 1
  static Integer usum(const Integer & a, const Integer & b, bool subtract, bool oneDigitOverflow = false);
  static IntegerDivision udiv(const Integer & a, const Integer & b);
  Integer multiplyByPowerOf2(uint8_t pow) const;
  Integer divideByPowerOf2(uint8_t pow) const;
  Integer multiplyByPowerOfBase(uint8_t pow) const;

  // HalfDigits
  uint16_t numberOfHalfDigits() const {
    if (m_numberOfDigits == 0) { return 0; }
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

  bool usesImmediateDigit() const { return m_numberOfDigits == 1; }
  native_uint_t digit(uint8_t i) const {
    assert(i >= 0 && i < m_numberOfDigits);
    return (usesImmediateDigit() ? m_digit : m_digits[i]);
  }

  /* An integer can have (k_maxNumberOfDigits + 1) digits: either when it is an
   * overflow, or when we want to have one more digit than usual to compute a
   * big division. */
  bool isOverflow() const { return m_numberOfDigits == k_maxNumberOfDigits + 1 && m_digits == nullptr; }

  bool m_negative;
  uint8_t m_numberOfDigits; // In base native_uint_t
  union {
    native_uint_t * m_digits; // Little-endian
    native_uint_t m_digit;
  };
};

struct IntegerDivision {
  Integer quotient;
  Integer remainder;
};

}

#endif
