#ifndef POINCARE_NUMBER_H
#define POINCARE_NUMBER_H

#include "integer.h"
#include "old_expression.h"

namespace Poincare {

/* Number class has 5 subclasses:
 * - Undefined
 * - Nonreal
 * - BasedInteger
 * - Rational
 * - Float
 * - Decimal
 * - Infinity
 *
 * Before being approximated, an expression should be reduced and thus should
 * not contain any Decimal. */

class Rational;

class NumberNode : public ExpressionNode {
 public:
  int numberOfChildren() const override {
    assert(isNumber());
    return 0;
  }
  virtual void setNegative(bool negative) = 0;

  // Basic test
  virtual bool isZero() const = 0;
  virtual bool isOne() const = 0;
  virtual bool isMinusOne() const = 0;
  virtual bool isInteger() const = 0;
  virtual Integer integerValue() const = 0;

  OMG::Troolean isNull(Context* context) const override {
    return OMG::BoolToTroolean(isZero());
  }

  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue) override;
};

class Number : public OExpression {
 public:
  Number(const NumberNode* node) : OExpression(node) {}
  /* Return either a Rational, a Decimal or an Infinity. */
  static Number ParseNumber(const char* integralPart, size_t integralLength,
                            const char* decimalPart, size_t decimalLength,
                            bool exponentIsNegative, const char* exponentPart,
                            size_t exponentLength);
  /* Return either a Decimal or an Infinity or an Undefined. */
  template <typename T>
  static Number DecimalNumber(T f);
  /* Return either a Float or an Infinity or an Undefined */
  template <typename T>
  static Number FloatNumber(T f);
  /* This set of Functions return either a Rational or a Float
   * or Infinity in case of overflow. Decimal are not taken into
   * account as it is not an internal node - it will always be turned into a
   * Rational/Float beforehand. */
  static Number Addition(const Number& i, const Number& j);
  static Number Multiplication(const Number& i, const Number& j);
  static Number Power(const Number& i, const Number& j);
  static int NaturalOrder(const Number& i, const Number& j);

  /* Number::isPositive() does not need a context or an angle unit
   * (a number can be Infinity, Undefined, Float, Decimal, Rational). */
  OMG::Troolean isPositive() const { return OExpression::isPositive(nullptr); }
  Number setSign(bool positive) {
    node()->setNegative(!positive);
    return *this;
  }

  bool derivate(const ReductionContext& reductionContext, Symbol symbol,
                OExpression symbolValue);

  bool isZero() const { return node()->isZero(); }
  bool isOne() const { return node()->isOne(); }
  bool isMinusOne() const { return node()->isMinusOne(); }
  bool isInteger() const { return node()->isInteger(); }
  Integer integerValue() const {
    assert(isInteger());
    return node()->integerValue();
  }

 protected:
  Number() : OExpression() {}
  NumberNode* node() const {
    return static_cast<NumberNode*>(OExpression::node());
  }

 private:
  typedef Rational (*RationalBinaryOperation)(const Rational& i,
                                              const Rational& j);
  typedef double (*DoubleBinaryOperation)(double i, double j);
  static Number BinaryOperation(const Number& i, const Number& j,
                                RationalBinaryOperation rationalOp,
                                DoubleBinaryOperation doubleOp);
};

}  // namespace Poincare

#endif
