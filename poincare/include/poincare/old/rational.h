#ifndef POINCARE_RATIONAL_H
#define POINCARE_RATIONAL_H

#include "complex.h"
#include "integer.h"
#include "number.h"

namespace Poincare {

class RationalNode final : public NumberNode {
 public:
  RationalNode(const native_uint_t* i, uint8_t numeratorSize,
               const native_uint_t* j, uint8_t denominatorSize, bool negative);

  Integer signedNumerator() const;
  Integer unsignedNumerator() const;
  Integer denominator() const;
  bool isNegative() const { return m_negative; }
  void setNegative(bool negative) override { m_negative = negative; }

  // PoolObject
  size_t size() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Rational";
  }
  void logAttributes(std::ostream& stream) const override;
#endif

  // SerializationHelperInterface
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  // OExpression subclassing
  Type otype() const override { return Type::Rational; }
  OMG::Troolean isPositive(Context* context) const override {
    return OMG::BoolToTroolean(!m_negative);
  }

  // Basic test
  bool isZero() const override { return unsignedNumerator().isZero(); }
  bool isOne() const override {
    return signedNumerator().isOne() && isInteger();
  }
  bool isTwo() const { return signedNumerator().isTwo() && isInteger(); }
  bool isThree() const { return signedNumerator().isThree() && isInteger(); }
  bool isMinusOne() const override {
    return signedNumerator().isMinusOne() && isInteger();
  }
  bool isMinusTwo() const {
    return signedNumerator().isMinusTwo() && isInteger();
  }
  bool isHalf() const {
    return signedNumerator().isOne() && denominator().isTwo();
  }
  bool isMinusHalf() const {
    return signedNumerator().isMinusOne() && denominator().isTwo();
  }
  bool isTen() const { return signedNumerator().isTen() && isInteger(); }
  bool isGreaterThanOne() const {
    return denominator().isLowerThan(signedNumerator());
  }
  bool isInteger() const override { return denominator().isOne(); }
  Integer integerValue() const override {
    assert(isInteger());
    return signedNumerator();
  }

  static int NaturalOrder(const RationalNode* i, const RationalNode* j);

 private:
  int simplificationOrderSameType(const ExpressionNode* e, bool ascending,
                                  bool ignoreParentheses) const override;
  OExpression shallowBeautify(
      const ReductionContext& reductionContext) override;
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    assert(!m_negative);
    return isInteger() ? LayoutShape::Integer : LayoutShape::Fraction;
  };
  bool m_negative;
  uint8_t m_numberOfDigitsNumerator;
  uint8_t m_numberOfDigitsDenominator;
  native_uint_t m_digits[0];
};

class Rational final : public Number {
  friend class ExpressionNode;
  friend class RationalNode;
  friend class PowerNode;
  friend class Power;

 public:
  /* The constructor build a irreductible fraction */
  Rational(const RationalNode* node) : Number(node) {}
  static Rational Builder(Integer& num, Integer& den);
  static Rational Builder(const Integer& numerator);
  static Rational Builder(native_int_t i);
  static Rational Builder(native_int_t i, native_int_t j);
  static Rational Builder(const char* iString, const char* jString);

  // PoolObject
  RationalNode* node() const {
    return static_cast<RationalNode*>(Number::node());
  }

  // Properties
  Integer signedIntegerNumerator() const { return node()->signedNumerator(); }
  Integer unsignedIntegerNumerator() const {
    return node()->unsignedNumerator();
  }
  Integer integerDenominator() const { return node()->denominator(); }

  // BasicTest
  bool isNegative() const { return node()->isNegative(); }
  bool isTwo() const { return node()->isTwo(); }
  bool isThree() const { return node()->isThree(); }
  bool isMinusTwo() const { return node()->isMinusTwo(); }
  bool isHalf() const { return node()->isHalf(); }
  bool isMinusHalf() const { return node()->isMinusHalf(); }
  bool isTen() const { return node()->isTen(); }
  bool isGreaterThanOne() const { return node()->isGreaterThanOne(); }
  bool numeratorOrDenominatorIsInfinity() const;

  // Arithmetic
  /* Warning: when using this function, always assert that the result does not
   * involve infinity numerator or denominator or handle these cases. */
  static Rational Addition(const Rational& i, const Rational& j);
  static Rational Multiplication(const Rational& i, const Rational& j);
  // IntegerPower of (p1/q1)^(p2) --> (p1^p2)/(q1^p2)
  static Rational IntegerPower(const Rational& i, const Integer& j);
  static int NaturalOrder(const Rational& i, const Rational& j) {
    return RationalNode::NaturalOrder(i.node(), j.node());
  }

  // Simplification
  OExpression shallowReduce();

 private:
  static Rational Builder(const native_uint_t* i, uint8_t numeratorSize,
                          const native_uint_t* j, uint8_t denominatorSize,
                          bool negative);

  /* Simplification */
  OExpression shallowBeautify();
};

}  // namespace Poincare

#endif
