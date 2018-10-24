#include <poincare/division.h>
#include <poincare/fraction_layout.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <cmath>
#include <assert.h>
#include <string.h>
#include <float.h>

namespace Poincare {

int DivisionNode::polynomialDegree(Context & context, const char * symbolName) const {
  if (childAtIndex(1)->polynomialDegree(context, symbolName) != 0) {
    return -1;
  }
  return childAtIndex(0)->polynomialDegree(context, symbolName);
}

bool DivisionNode::childNeedsParenthesis(const TreeNode * child) const {
  if (static_cast<const ExpressionNode *>(child)->isNumber() && static_cast<const ExpressionNode *>(child)->sign() == Sign::Negative) {
    return true;
  }
  if (static_cast<const ExpressionNode *>(child)->type() == Type::Rational && !static_cast<const RationalNode *>(child)->denominator().isOne()) {
    return true;
  }
  Type types[] = {Type::Subtraction, Type::Opposite, Type::Multiplication, Type::Division, Type::Addition};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 5);
}

Layout DivisionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  const ExpressionNode * numerator = childAtIndex(0)->type() == Type::Parenthesis ? childAtIndex(0)->childAtIndex(0) : childAtIndex(0);
  const ExpressionNode * denominator = childAtIndex(1)->type() == Type::Parenthesis ? childAtIndex(1)->childAtIndex(0) : childAtIndex(1);
  return FractionLayout(numerator->createLayout(floatDisplayMode, numberOfSignificantDigits), denominator->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int DivisionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "/");
}

Expression DivisionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return Division(this).shallowReduce(context, angleUnit, replaceSymbols);
}

template<typename T> Complex<T> DivisionNode::compute(const std::complex<T> c, const std::complex<T> d) {
  return Complex<T>(c/d);
}

template<typename T> MatrixComplex<T> DivisionNode::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n) {
  MatrixComplex<T> inverse = n.inverse();
  MatrixComplex<T> result = MultiplicationNode::computeOnComplexAndMatrix<T>(c, inverse);
  return result;
}

template<typename T> MatrixComplex<T> DivisionNode::computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n) {
  if (m.numberOfColumns() != n.numberOfColumns()) {
    return MatrixComplex<T>::Undefined();
  }
  MatrixComplex<T> inverse = n.inverse();
  MatrixComplex<T> result = MultiplicationNode::computeOnMatrices<T>(m, inverse);
  return result;
}

// Division

Division::Division() : Expression(TreePool::sharedPool()->createTreeNode<DivisionNode>()) {}

Expression Division::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression p = Power(childAtIndex(1), Rational(-1));
  Multiplication m = Multiplication(childAtIndex(0), p);
  p.shallowReduce(context, angleUnit); // Imagine Division(2,1). p would be 1^(-1) which can be simplified
  replaceWithInPlace(m);
  return m.shallowReduce(context, angleUnit);
}

}
