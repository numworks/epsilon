#include <poincare/subtraction.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/addition.h>
#include <poincare/multiplication.h>
#include <poincare/opposite.h>
#include <poincare/rational.h>
#include <assert.h>

namespace Poincare {

int SubtractionNode::polynomialDegree(Context & context, const char * symbolName) const {
  int degree = 0;
  for (ExpressionNode * e : children()) {
    int d = e->polynomialDegree(context, symbolName);
    if (d < 0) {
      return -1;
    }
    degree = d > degree ? d : degree;
  }
  return degree;
}

// Private

ComplexCartesian SubtractionNode::complexCartesian(Context & context, Preferences::AngleUnit angleUnit) const {
  Subtraction e(this);
  ComplexCartesian cartesian0 = e.childAtIndex(0).complexCartesian(context, angleUnit);
  ComplexCartesian cartesian1 = e.childAtIndex(1).complexCartesian(context, angleUnit);
  if (cartesian0.isUninitialized() || cartesian1.isUninitialized()) {
    return ComplexCartesian();
  }
  return ComplexCartesian::Builder(
    Subtraction(cartesian0.real(), cartesian1.real()).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation),
    Subtraction(cartesian0.imag(), cartesian1.imag()).shallowReduce(context, angleUnit, ReductionTarget::BottomUpComputation)
  );
}

bool SubtractionNode::childNeedsParenthesis(const TreeNode * child) const {
  if (child == childAtIndex(0)) {
    return false;
  }
  if (static_cast<const ExpressionNode *>(child)->isNumber() && Number(static_cast<const NumberNode *>(child)).sign() == Sign::Negative) {
    return true;
  }
  Type types[] = {Type::Subtraction, Type::Opposite, Type::Addition};
  return static_cast<const ExpressionNode *>(child)->isOfType(types, 3);
}

Layout SubtractionNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Infix(Subtraction(this), floatDisplayMode, numberOfSignificantDigits, "-");
}

int SubtractionNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
    return SerializationHelper::Infix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "-");
}

template<typename T> MatrixComplex<T> SubtractionNode::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m) {
  MatrixComplex<T> opposite = computeOnMatrixAndComplex(m, c);
  MatrixComplex<T> result;
  for (int i = 0; i < opposite.numberOfChildren(); i++) {
    result.addChildAtIndexInPlace(OppositeNode::compute(opposite.complexAtIndex(i)), i, i);
  }
  result.setDimensions(opposite.numberOfRows(), opposite.numberOfColumns());
  return result;
}

Expression SubtractionNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Subtraction(this).shallowReduce(context, angleUnit, target);
}

Subtraction::Subtraction() : Expression(TreePool::sharedPool()->createTreeNode<SubtractionNode>()) {}

Expression Subtraction::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression e = Expression::defaultShallowReduce(context, angleUnit);
  if (e.isUndefined()) {
    return e;
  }
  Expression m = Multiplication(Rational(-1), childAtIndex(1));
  Addition a(childAtIndex(0), m);
  m = m.shallowReduce(context, angleUnit, target);
  replaceWithInPlace(a);
  return a.shallowReduce(context, angleUnit, target);
}

}
