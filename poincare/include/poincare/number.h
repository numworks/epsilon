#ifndef POINCARE_NUMBER_H
#define POINCARE_NUMBER_H

#include <poincare/expression_reference.h>

namespace Poincare {

/* Number class has 3 subclasses:
 * - Undefined
 * - Integer
 * - Rational
 * - Float
 * - Decimal
 * - Infinite
 */

class IntegerReference;
class RationalReference;

class NumberNode : public ExpressionNode {
public:
  bool isNumber() const override { return true; }
  int numberOfChildren() const override { return 0; }

  double doubleApproximation() const;
};

class NumberReference : public ExpressionReference {
public:
  using ExpressionReference::ExpressionReference;
  NumberNode * numberNode() const { assert(!isAllocationFailure()); return static_cast<NumberNode *>(node()); }
  /* Return either a IntegerReference, a DecimalReference or an InfiniteReference. */
  static NumberReference Integer(const char * digits, size_t length, bool negative);
  /* Return either a DecimalInteger or an InfiniteReference. */
  template <typename T> static NumberReference Decimal(T f);
  /* This set of Functions return either a RationalReference or a FloatReference
   * or InfiniteReference in case of overflow. DecimalReference are not taken into
   * account as it is not an internal node - it will always be turned into a
   * Rational/Float beforehand. */
  static NumberReference Addition(const NumberReference i, const NumberReference j);
  static NumberReference Multiplication(const NumberReference i, const NumberReference j);
  static NumberReference Power(const NumberReference i, const NumberReference j);
private:
  typedef IntegerReference (*IntegerBinaryOperation)(const IntegerReference, const IntegerReference);
  typedef RationalReference (*RationalBinaryOperation)(const RationalReference, const RationalReference);
  typedef double (*DoubleBinaryOperation)(double, double);
  static NumberReference BinaryOperation(const NumberReference i, const NumberReference j, IntegerBinaryOperation integerOp, RationalBinaryOperation rationalOp, DoubleBinaryOperation doubleOp);
};

}

#endif
