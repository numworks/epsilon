extern "C" {
#include <assert.h>
#include <string.h>
#include <float.h>
}

#include <poincare/division.h>
#include <poincare/power.h>
#include <poincare/rational.h>
//#include <poincare/tangent.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <cmath>
#include <poincare/fraction_layout_node.h>

namespace Poincare {

DivisionNode * DivisionNode::FailedAllocationStaticNode() {
  static AllocationFailureExpressionNode<DivisionNode> failure;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failure);
  return &failure;
}

int DivisionNode::polynomialDegree(char symbolName) const {
  if (childAtIndex(1)->polynomialDegree(symbolName) != 0) {
    return -1;
  }
  return childAtIndex(0)->polynomialDegree(symbolName);
}

bool DivisionNode::needsParenthesesWithParent(const SerializationHelperInterface * e) const {
  Type types[] = {Type::Division, Type::Power, Type::Factorial};
  return static_cast<const ExpressionNode *>(e)->isOfType(types, 3);
}

LayoutRef DivisionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  const ExpressionNode * numerator = childAtIndex(0)->type() == Type::Parenthesis ? childAtIndex(0)->childAtIndex(0) : childAtIndex(0);
  const ExpressionNode * denominator = childAtIndex(1)->type() == Type::Parenthesis ? childAtIndex(1)->childAtIndex(0) : childAtIndex(1);
  return FractionLayoutRef(numerator->createLayout(floatDisplayMode, numberOfSignificantDigits), denominator->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

Expression DivisionNode::shallowReduce(Context& context, Preferences::AngleUnit angleUnit) const {
  return Division(this).shallowReduce(context, angleUnit);
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

Expression Division::shallowReduce(Context & context, Preferences::AngleUnit angleUnit) const {
  Expression result = Expression::defaultShallowReduce(context, angleUnit);
  if (result.isUndefinedOrAllocationFailure()) {
    return result;
  }
  Expression p = Power(childAtIndex(1), Rational(-1));
  p = p.shallowReduce(context, angleUnit); // Imagine Division(2,1). p would be 1^(-1) which can be simplified
  Multiplication m = Multiplication(childAtIndex(0), p);
  return m.shallowReduce(context, angleUnit);
}

}
