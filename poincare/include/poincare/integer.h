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
class Integer;
struct IntegerDivision;

typedef uint16_t half_native_uint_t;
typedef int32_t native_int_t;
typedef int64_t double_native_int_t;
typedef uint32_t native_uint_t;
typedef uint64_t double_native_uint_t;

/* All algorithm should be improved with:
 * Modern Computer Arithmetic, Richard P. Brent and Paul Zimmermann */

class NaturalIntegerAbstract {
friend class Integer;
friend class Rational;
public:
  // Getters
  uint32_t digit(int i) const { assert(i < m_numberOfDigits); return digits()[i]; }
  size_t numberOfDigits() const { return m_numberOfDigits; }

  // Layout
  int serialize(char * buffer, int bufferSize) const;
  LayoutReference createLayout() const;

  // Approximation
  template<typename T> T approximate() const;

  // Properties
  static int NumberOfBase10Digits(const NaturalIntegerAbstract * i);
  virtual bool isOne() const { return (m_numberOfDigits == 1 && digit(0) == 1); };
  virtual bool isTwo() const { return (m_numberOfDigits == 1 && digit(0) == 2); };
  virtual bool isTen() const { return (m_numberOfDigits == 1 && digit(0) == 10); };
  virtual bool isZero() const { return (m_numberOfDigits == 0); };
  bool isInfinity() const { return m_numberOfDigits > k_maxNumberOfDigits; }
  bool isEven() const { return (isZero() || ((digit(0) & 1) == 0)); }

  // Arithmetic
  /* buffer has to be k_maxNumberOfDigits+1 to allow ...*/
  static int8_t ucmp(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b); // -1, 0, or 1
  static Integer usum(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b, bool subtract);
  static Integer umult(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b);
  static IntegerDivision udiv(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b);
  static Integer upow(const NaturalIntegerAbstract * a, const NaturalIntegerAbstract * b);
  static Integer ufact(const NaturalIntegerAbstract * a);

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
  static Integer IntegerWithHalfDigitAtIndex(half_native_uint_t halfDigit, int index);

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
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override { return Integer(this).shallowReduce(context, angleUnit); }

  // Approximation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<float>(templatedApproximate<float>()); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return Complex<double>(templatedApproximate<double>()); }
  template<typename T> T templatedApproximate() const;

  // Layout
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode = Preferences::PrintFloatMode::Decimal, int numberOfSignificantDigits = 0) const override;

  // TreeNode
  size_t size() const override;
#if TREE_LOG
  const char * description() const override { return "Integer";  }
#endif

  // ExpressionNode
  Sign sign() const override { return m_negative ? Sign::Negative : Sign::Positive; }
  Expression setSign(Sign s, Context & context, Preferences::AngleUnit angleUnit) override { Integer(this).setSign(s, context, angleUnit); }

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

class AllocationFailureIntegerNode : public IntegerNode, public AllocationFailedExpressionNode {
public
  // ExpressionNode
  using AllocationFailedExpressionNode::type;
  using AllocationFailedExpressionNode::approximate;
  using AllocationFailedExpressionNode::writeTextInBuffer;
  using AllocationFailedExpressionNode::createLayout;
  using AllocationFailedExpressionNode::numberOfChildren;
  using AllocationFailedExpressionNode::isAllocationFailure;
  size_t size() const override { return sizeof(AllocationFailureIntegerNode); }
#if TREE_LOG
  const char * description() const override { return "AllocationFailureIntegerNode";  }
#endif

  // IntegerNode
  bool isZero() const override { return false; }
  void setDigits(native_int_t i) override {}
  void setDigits(double_native_int_t i);
  void setDigits(const native_uint_t * digits, size_t size, bool negative);
};

class Integer : public Number {
friend class NaturalIntegerAbstract;
friend class NaturalIntegerPointer;
friend class IntegerNode;
friend class Rational;
friend class Decimal;
public:
  Integer(TreeNode * n) : Number(n) {}
  Integer(const char * digits, size_t length, bool negative);
  Integer(const NaturalIntegerAbstract * naturalInteger);
  Integer(native_int_t i);
  Integer(double_native_int_t i);
  static Integer Overflow() { return Integer((native_uint_t *)nullptr, IntegerNode::k_maxNumberOfDigits+1, false); }

  constexpr static int k_maxExtractableInteger = 0x7FFFFFFF;
  int extractedInt() const;

   // Comparison
  static int NaturalOrder(const Integer i, const Integer j);

  // Properties
  virtual bool isZero() const;
  bool isOne() const;
  bool isInfinity() const;
  bool isEven() const;
  bool isNegative() const { return node()->sign() == ExpressionNode::Sign::Negative; }
  void setNegative(bool negative);
  static int NumberOfBase10Digits(const Integer i);

  // Arithmetic
  static Integer Addition(const Integer i, const Integer j);
  static Integer Subtraction(const Integer i, const Integer j);
  static Integer Multiplication(const Integer i, const Integer j);
  static IntegerDivision Division(const Integer numerator, const Integer denominator);
  static Integer Power(const Integer i, const Integer j);
  static Integer Factorial(const Integer i);
private:
  // TreeNode
  IntegerNode * typedNode() const { assert(node()->type() == ExpressionNode::Type::Integer); return static_cast<IntegerNode *>(node()); }

  Integer(const native_uint_t * digits, size_t numberOfDigits, bool negative);
  Integer(size_t size) : Number(TreePool::sharedPool()->createTreeNode<IntegerNode>(size)) {
  }
  static Integer addition(const Integer a, const Integer b, bool inverseBNegative);
  size_t numberOfDigits() const { return typedNode()->numberOfDigits(); }
  uint32_t digit(int i) const { return typedNode()->digit(i); }

  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

struct IntegerDivision {
  Integer quotient;
  Integer remainder;
};

}

#endif
