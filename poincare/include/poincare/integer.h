#ifndef POINCARE_INTEGER_H
#define POINCARE_INTEGER_H

#include <stdint.h>
#include <assert.h>
#include <poincare/number.h>
#include <poincare/complex.h>

namespace Poincare {

class ExpressionLayout;
class LayoutReference;
class LayoutNode;
class IntegerReference;
struct IntegerDivisionReference;

typedef uint16_t half_native_uint_t;
typedef int32_t native_int_t;
typedef int64_t double_native_int_t;
typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

/* All algorithm should be improved with:
 * Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann */

class NaturalIntegerAbstract {
friend class IntegerReference;
friend class RationalReference;
public:
  // Getters
  uint32_t digit(int i) const { assert(i < m_numberOfDigits); return digits()[i]; }
  size_t numberOfDigits() const { return m_numberOfDigits; }

  // Layout
  int writeTextInBuffer(char * buffer, int bufferSize) const;
  LayoutReference createLayout() const;

  // Approximation
  template<typename T> T approximate() const;

  // Properties
  static int NumberOfBase10Digits(const NaturalIntegerAbstract * i);
  virtual bool isOne() const { return (m_numberOfDigits == 1 && digit(0) == 1); };
  virtual bool isTwo() const { return (m_numberOfDigits == 1 && digit(0) == 2); };
  virtual bool isTen() const { return (m_numberOfDigits == 1 && digit(0) == 10); };
  bool isZero() const { return (m_numberOfDigits == 0); };
  bool isInfinity() const { return m_numberOfDigits > k_maxNumberOfDigits; }
  bool isEven() const { return (m_numberOfDigits == 0 || ((digit(0) & 1) == 0)); }

  // Arithmetic
  /* buffer has to be k_maxNumberOfDigits+1 to allow ...*/
  static int8_t ucmp(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b); // -1, 0, or 1
  static IntegerReference usum(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b, bool subtract);
  static IntegerReference umult(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b);
  static IntegerDivisionReference udiv(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b);
  static IntegerReference upow(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b);
  static IntegerReference ufact(const NaturalIntegerAbstract * a);

  constexpr static int k_maxNumberOfDigits = 32;
protected:
  constexpr static int k_maxNumberOfDigitsBase10 = 308; // (2^32)^k_maxNumberOfDigits ~ 1E308
  NaturalIntegerAbstract(size_t numberOfDigits = 0) :
    m_numberOfDigits(numberOfDigits)
  {
  }
  virtual native_uint_t * digits() const = 0;
  virtual void setDigitAtIndex(native_uint_t digit, int index) = 0;

  size_t m_numberOfDigits; // In base native_uint_max
private:
  static IntegerReference IntegerWithHalfDigitAtIndex(half_native_uint_t halfDigit, int index);

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
    return ((half_native_uint_t *)digits())[i];
  }

  static_assert(sizeof(double_native_int_t) <= sizeof(double_native_uint_t), "double_native_int_t type has not the right size compared to double_native_uint_t");
  static_assert(sizeof(native_int_t) == sizeof(native_uint_t), "native_int_t type has not the right size compared to native_uint_t");
  static_assert(sizeof(double_native_uint_t) == 2*sizeof(native_uint_t), "double_native_uint_t should be twice the size of native_uint_t");
  static_assert(sizeof(double_native_int_t) == 2*sizeof(native_int_t), "double_native_int_t type has not the right size compared to native_int_t");
};

class NaturalIntegerPointer : public NaturalIntegerAbstract {
public:
  NaturalIntegerPointer(native_uint_t * buffer, size_t size);
private:
  native_uint_t * digits() const override { return m_digits; }
  void setDigitAtIndex(native_uint_t digit, int index) override { assert(index < m_numberOfDigits); m_digits[index] = digit; }
  native_uint_t * m_digits;
};

class IntegerNode : public NumberNode, public NaturalIntegerAbstract {
public:
  IntegerNode() :
    NaturalIntegerAbstract(),
    m_negative(false) {}
  void setDigits(native_int_t i);
  void setDigits(double_native_int_t i);
  void setDigits(const native_uint_t * digits, size_t size, bool negative);

  // Getters
  native_uint_t * digits() const override { return (native_uint_t *)m_digits; }

  // ExpressionNode
  Type type() const override { return Type::Integer; }

  // Simplification
  ExpressionReference shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;

  // Approximation
  EvaluationReference<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return ComplexReference<float>(templatedApproximate<float>()); }
  EvaluationReference<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return ComplexReference<double>(templatedApproximate<double>()); }
  template<typename T> T templatedApproximate() const;

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // TreeNode
  size_t size() const override;
#if TREE_LOG
  const char * description() const override { return "Integer";  }
#endif

  // ExpressionNode
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }
  ExpressionReference setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override;

  void setNegative(bool negative);

  // Comparison
  static int NaturalOrder(const IntegerNode * i, const IntegerNode * j);
  int simplificationOrderSameType(const ExpressionNode * e, bool canBeInterrupted) const override {  return NaturalOrder(this, static_cast<const IntegerNode *>(e)); }

  // Properties
  bool isOne() const override { return (NaturalIntegerAbstract::isOne() && !m_negative); };
  bool isTwo() const override { return (NaturalIntegerAbstract::isTwo() && !m_negative); };
  bool isTen() const override{ return (NaturalIntegerAbstract::isTen() && !m_negative); };
  bool isMinusOne() const { return (NaturalIntegerAbstract::isOne() && m_negative); };
private:
  void setDigitAtIndex(native_uint_t digit, int index) override { m_digits[index] = digit; }
  bool m_negative; // Make sure zero cannot be negative
  native_uint_t m_digits[0];
};


class IntegerReference : public NumberReference {
friend class NaturalIntegerAbstract;
friend class NaturalIntegerPointer;
friend class IntegerNode;
friend class RationalReference;
friend class DecimalReference;
public:
  IntegerReference(TreeNode * n) : NumberReference(n) {}
  IntegerReference(const char * digits, size_t length, bool negative);
  IntegerReference(const NaturalIntegerAbstract * naturalInteger);
  IntegerReference(native_int_t i);
  IntegerReference(double_native_int_t i);
  static IntegerReference Overflow() { return IntegerReference((native_uint_t *)nullptr, IntegerNode::k_maxNumberOfDigits+1, false); }

  constexpr static int k_maxExtractableInteger = 0x7FFFFFFF;
  int extractedInt() const;

   // Comparison
  static int NaturalOrder(const IntegerReference i, const IntegerReference j);

  // Properties
  bool isZero() const;
  bool isOne() const;
  bool isInfinity() const;
  bool isEven() const;
  bool isNegative() const { return node()->sign() == ExpressionNode::Sign::Negative; }
  void setNegative(bool negative);
  static int NumberOfBase10Digits(const IntegerReference i);

  // Arithmetic
  static IntegerReference Addition(const IntegerReference i, const IntegerReference j);
  static IntegerReference Subtraction(const IntegerReference i, const IntegerReference j);
  static IntegerReference Multiplication(const IntegerReference i, const IntegerReference j);
  static IntegerDivisionReference Division(const IntegerReference numerator, const IntegerReference denominator);
  static IntegerReference Power(const IntegerReference i, const IntegerReference j);
  static IntegerReference Factorial(const IntegerReference i);
private:
  // TreeNode
  IntegerNode * typedNode() const { assert(node()->type() == ExpressionNode::Type::Integer); return static_cast<IntegerNode *>(node()); }

  IntegerReference(const native_uint_t * digits, size_t numberOfDigits, bool negative);
  IntegerReference(size_t size) : NumberReference(TreePool::sharedPool()->createTreeNode<IntegerNode>(size)) {
  }
  static IntegerReference addition(const IntegerReference a, const IntegerReference b, bool inverseBNegative);
  size_t numberOfDigits() const { return typedNode()->numberOfDigits(); }
  uint32_t digit(int i) const { return typedNode()->digit(i); }
};

struct IntegerDivisionReference {
  IntegerReference quotient;
  IntegerReference remainder;
};

}

#endif
