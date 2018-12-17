#ifndef POINCARE_NUMBER_H
#define POINCARE_NUMBER_H

#include <poincare/expression.h>

namespace Poincare {

/* Number class has 5 subclasses:
 * - Undefined
 * - Rational
 * - Float
 * - Decimal
 * - Infinity
 */

class Rational;

class NumberNode : public ExpressionNode {
public:
  bool isNumber() const override { return true; }
  int numberOfChildren() const override { return 0; }

  double doubleApproximation() const;
};

class Number : public Expression {
public:
  Number(const NumberNode * node) : Expression(node) {}
  /* Return either a Rational, a Decimal or an Infinity. */
  static Number ParseNumber(const char * integralPart, size_t integralLength, const char * decimalPart, size_t decimalLenght, bool exponentIsNegative, const char * exponentPart, size_t exponentLength);
  /* Return either a Decimal or an Infinity or an Undefined. */
  template <typename T> static Number DecimalNumber(T f);
  /* Return either a Float or an Infinity or an Undefined */
  static Number FloatNumber(double f);
  /* This set of Functions return either a Rational or a Float
   * or Infinity in case of overflow. Decimal are not taken into
   * account as it is not an internal node - it will always be turned into a
   * Rational/Float beforehand. */
  static Number Addition(const Number & i, const Number & j);
  static Number Multiplication(const Number & i, const Number & j);
  static Number Power(const Number & i, const Number & j);
  static int NaturalOrder(const Number & i, const Number & j);

  Number setSign(ExpressionNode::Sign s, Context & context, Preferences::AngleUnit angleUnit) { return Expression::setSign(s, context, angleUnit).convert<Number>(); }
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
