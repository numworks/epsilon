#include <poincare/matrix_transpose.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixTranspose::s_functionHelper;

int MatrixTransposeNode::numberOfChildren() const { return MatrixTranspose::s_functionHelper.numberOfChildren(); }

Expression MatrixTransposeNode::shallowReduce(ReductionContext reductionContext) {
  return MatrixTranspose(this).shallowReduce();
}

Layout MatrixTransposeNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixTranspose(this), floatDisplayMode, numberOfSignificantDigits, MatrixTranspose::s_functionHelper.name());
}

int MatrixTransposeNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixTranspose::s_functionHelper.name());
}

template<typename T>
Evaluation<T> MatrixTransposeNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Evaluation<T> transpose;
  if (input.type() == EvaluationNode<T>::Type::MatrixComplex) {
    transpose = static_cast<MatrixComplex<T>&>(input).transpose();
  } else {
    transpose = input;
  }
  assert(!transpose.isUninitialized());
  return transpose;
}


Expression MatrixTranspose::shallowReduce() {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix transpose = static_cast<Matrix&>(c).createTranspose();
    return transpose;
  }
  if (!c.recursivelyMatches(Expression::IsMatrix)) {
    return c;
  }
  return *this;
#else
  if (c.type() != ExpressionNode::Type::Matrix) {
    replaceWithInPlace(c);
    return c;
  }
  return *this;
#endif
}

}
