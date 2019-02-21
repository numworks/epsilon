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

template<typename T> MatrixComplex<T> SubtractionNode::computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> m, Preferences::ComplexFormat complexFormat) {
  MatrixComplex<T> opposite = computeOnMatrixAndComplex(m, c, complexFormat);
  MatrixComplex<T> result = MatrixComplex<T>::Builder();
  for (int i = 0; i < opposite.numberOfChildren(); i++) {
    result.addChildAtIndexInPlace(OppositeNode::compute(opposite.complexAtIndex(i), complexFormat), i, i);
  }
  result.setDimensions(opposite.numberOfRows(), opposite.numberOfColumns());
  return result;
}

Expression SubtractionNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target) {
  return Subtraction(this).shallowReduce(context, complexFormat, angleUnit, target);
}

Subtraction Subtraction::Builder() {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(SubtractionNode));
  SubtractionNode * node = new (bufferNode) SubtractionNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node);
  return static_cast<Subtraction &>(h);
}

Subtraction Subtraction::Builder(Expression child0, Expression child1) {
  Subtraction d = Subtraction::Builder();
  d.replaceChildAtIndexInPlace(0, child0);
  d.replaceChildAtIndexInPlace(1, child1);
  return d;
}

Expression Subtraction::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) {
  Expression e = Expression::defaultShallowReduce();
  if (e.isUndefined()) {
    return e;
  }
  Expression m = Multiplication::Builder(Rational::Builder(-1), childAtIndex(1));
  Addition a = Addition::Builder(childAtIndex(0), m);
  m = m.shallowReduce(context, complexFormat, angleUnit, target);
  replaceWithInPlace(a);
  return a.shallowReduce(context, complexFormat, angleUnit, target);
}

}
