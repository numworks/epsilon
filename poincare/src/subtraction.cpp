#include <poincare/subtraction.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/addition.h>
#include <poincare/multiplication_explicite.h>
#include <poincare/opposite.h>
#include <poincare/rational.h>
#include <assert.h>

namespace Poincare {

int SubtractionNode::polynomialDegree(Context * context, const char * symbolName) const {
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

bool SubtractionNode::childNeedsUserParentheses(const Expression & child) const {
  if (child.node() == childAtIndex(0)) {
    return false;
  }
  if (child.isNumber() && static_cast<const Number &>(child).sign() == Sign::Negative) {
    return true;
  }
  Type types[] = {Type::Subtraction, Type::Opposite, Type::Addition};
  return child.isOfType(types, 3);
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

Expression SubtractionNode::shallowReduce(ReductionContext reductionContext) {
  return Subtraction(this).shallowReduce(reductionContext);
}

Expression Subtraction::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  Expression e = Expression::defaultShallowReduce();
  if (e.isUndefined()) {
    return e;
  }
  Expression m = MultiplicationExplicite::Builder(Rational::Builder(-1), childAtIndex(1));
  Addition a = Addition::Builder(childAtIndex(0), m);
  m = m.shallowReduce(reductionContext);
  replaceWithInPlace(a);
  return a.shallowReduce(reductionContext);
}

}
