#include <poincare/division.h>
#include <poincare/fraction_layout.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/power.h>
#include <poincare/rational.h>
#include <poincare/addition.h>
#include <poincare/subtraction.h>
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
  if (static_cast<const ExpressionNode *>(child)->isNumber() && Number(static_cast<const NumberNode *>(child)).sign() == Sign::Negative) {
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

Expression DivisionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Division(this).shallowReduce(context, angleUnit, target);
}

ComplexCartesian DivisionNode::complexCartesian(Context & context, Preferences::AngleUnit angleUnit) const {
  Division e(this);
  ComplexCartesian cartesianChild0 = e.childAtIndex(0).complexCartesian(context, angleUnit);
  ComplexCartesian cartesianChild1 = e.childAtIndex(1).complexCartesian(context, angleUnit);
  if (cartesianChild0.isUninitialized() || cartesianChild1.isUninitialized()) {
    return ComplexCartesian();
  }
  Expression a = cartesianChild0.real();
  Expression b = cartesianChild0.imag();
  Expression c = cartesianChild1.real();
  Expression d = cartesianChild1.imag();
  assert(!a.isUninitialized() && !b.isUninitialized() && !c.isUninitialized() && !d.isUninitialized());
  Expression denominator = ComplexHelper::complexSquareNormComplexCartesian(c.clone(), d.clone(), context, angleUnit);
  return ComplexCartesian::Builder(
      Division(
        Addition(
          Multiplication(a.clone(), c.clone()).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
          Multiplication(b.clone(), d.clone()).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
        ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
        denominator.clone()
      ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
      Division(
        Subtraction(
          Multiplication(b, c).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
          Multiplication(a, d).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
        ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
        denominator
      ).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
    );
}

/*ComplexPolar DivisionNode::complexPolar(Context & context, Preferences::AngleUnit angleUnit) const {
  Division d(this);
  Expression r0 = d.childAtIndex(0).complexNorm(context, angleUnit);
  Expression r1 = d.childAtIndex(1).complexNorm(context, angleUnit);
  if (r0.isUninitialized() || r1.isUninitialized()) {
    return Expression();
  }
  return Division(r0,r1).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation);
}*/

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

Expression Division::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression p = Power(childAtIndex(1), Rational(-1));
  Multiplication m = Multiplication(childAtIndex(0), p);
  p.shallowReduce(context, angleUnit, target); // Imagine Division(2,1). p would be 1^(-1) which can be simplified
  replaceWithInPlace(m);
  return m.shallowReduce(context, angleUnit, target);
}

}
