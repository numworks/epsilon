#ifndef POINCARE_NUMBER_H
#define POINCARE_NUMBER_H

#include <poincare/expression.h>

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
  bool isNumber() const override { return true; }
  int numberOfChildren() const override { return 0; }
  virtual void setNegative(bool negative) = 0;

  double doubleApproximation() const;

  bool derivate(ReductionContext reductionContext, Symbol symbol, Expression symbolValue) override;

};

class Number : public Expression {
public:
  Number(const NumberNode * node) : Expression(node) {}
  /* Return either a Rational, a Decimal or an Infinity. */
  static Number ParseNumber(const char * integralPart, size_t integralLength, const char * decimalPart, size_t decimalLength, bool exponentIsNegative, const char * exponentPart, size_t exponentLength);
  /* Return either a Decimal or an Infinity or an Undefined. */
  template <typename T> static Number DecimalNumber(T f);
  /* Return either a Float or an Infinity or an Undefined */
  template <typename T> static Number FloatNumber(T f);
  /* This set of Functions return either a Rational or a Float
   * or Infinity in case of overflow. Decimal are not taken into
   * account as it is not an internal node - it will always be turned into a
   * Rational/Float beforehand. */
  static Number Addition(const Number & i, const Number & j);
  static Number Multiplication(const Number & i, const Number & j);
  static Number Power(const Number & i, const Number & j);
  static int NaturalOrder(const Number & i, const Number & j);

  /* Number::sign() does not need a context or an angle unit
   * (a number can be Infinity, Undefined, Float, Decimal, Rational). */
  ExpressionNode::Sign sign() const { return Expression::sign(nullptr); }
  Number setSign(ExpressionNode::Sign s) {
    assert(s == ExpressionNode::Sign::Positive || s == ExpressionNode::Sign::Negative);
    node()->setNegative(s == ExpressionNode::Sign::Negative);
    return *this;
  }

  bool derivate(ExpressionNode::ReductionContext reductionContext, Symbol symbol, Expression symbolValue);
protected:
  Number() : Expression() {}
  NumberNode * node() const { return static_cast<NumberNode *>(Expression::node()); }
private:
  typedef Rational (*RationalBinaryOperation)(const Rational & i, const Rational & j);
  typedef double (*DoubleBinaryOperation)(double i, double j);
  static Number BinaryOperation(const Number & i, const Number & j, RationalBinaryOperation rationalOp, DoubleBinaryOperation doubleOp);
};

}

#endif
