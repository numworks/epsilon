#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <poincare/static_hierarchy.h>
#include <stdint.h>
#include <assert.h>

namespace Poincare {

/* All algorithm should be improved with:
 * Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann */

// TODO: Integer should not be an expression!

struct IntegerDivision;

class Integer : public StaticHierarchy<0> {
public:
  typedef int32_t native_int_t;
  typedef uint32_t native_uint_t;
  typedef uint64_t double_native_uint_t;

  // FIXME: This constructor should be constexpr
  Integer(native_int_t i = 0) :
    StaticHierarchy<0>(),
    m_digit(i>0 ? i : -i),
    m_numberOfDigits(1),
    m_negative(i<0)
  {
  }
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

  // Expression subclassing
  Type type() const override;
  Expression * clone() const override;
  bool isEqualTo(const Integer & other) const;
  bool isLowerThan(const Integer & other) const;

  // Arithmetic
  static Integer Addition(const Integer & i, const Integer & j);
  static Integer Subtraction(const Integer & i, const Integer & j);
  static Integer Multiplication(const Integer & i, const Integer & j);
  static IntegerDivision Division(const Integer & numerator, const Integer & denominator);
  static Integer Power(const Integer & i, const Integer & j);
  //static Integer Division(const Integer & i, const Integer & j);
  //static IntegerDivision division(const Integer & i, const Integer & j);
  bool isOne() const { return (m_numberOfDigits == 1 && digit(0) == 1 && !m_negative); };
  bool isMinusOne() const { return (m_numberOfDigits == 1 && digit(0) == 1 && m_negative); };
  bool isZero() const { return (m_numberOfDigits == 1 && digit(0) == 0); };
private:
  Integer(const native_uint_t * digits, uint16_t numberOfDigits, bool negative);
  void releaseDynamicIvars();
  static int8_t ucmp(const Integer & a, const Integer & b); // -1, 0, or 1
  static Integer usum(const Integer & a, const Integer & b, bool subtract, bool outputNegative);
  static Integer addition(const Integer & a, const Integer & b, bool inverseBNegative);
  static IntegerDivision udiv(const Integer & a, const Integer & b);
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override;
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override;

  bool usesImmediateDigit() const { return m_numberOfDigits == 1; }
  native_uint_t digit(int i) const {
    assert(i >= 0 && i<m_numberOfDigits);
    return (usesImmediateDigit() ? m_digit : m_digits[i]);
  }

  /* Sorting */
  int compareToSameTypeExpression(const Expression * e) const override;

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
