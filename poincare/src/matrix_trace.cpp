#include <poincare/matrix_trace.h>
#include <poincare/addition.h>
#include <poincare/layout_helper.h>
#include <poincare/matrix.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/undefined.h>
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper MatrixTrace::s_functionHelper;

int MatrixTraceNode::numberOfChildren() const { return MatrixTrace::s_functionHelper.numberOfChildren(); }

Expression MatrixTraceNode::shallowReduce(Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ReductionTarget target, bool symbolicComputation) {
  return MatrixTrace(this).shallowReduce();
}

Layout MatrixTraceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(MatrixTrace(this), floatDisplayMode, numberOfSignificantDigits, MatrixTrace::s_functionHelper.name());
}

int MatrixTraceNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, MatrixTrace::s_functionHelper.name());
}

template<typename T>
Evaluation<T> MatrixTraceNode::templatedApproximate(Context& context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  Complex<T> result = Complex<T>::Builder(input.trace());
  return result;
}


Expression MatrixTrace::shallowReduce() {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c = childAtIndex(0);
#if MATRIX_EXACT_REDUCING
#if 0
  if (c.type() == ExpressionNode::Type::Matrix) {
    Matrix m = static_cast<Matrix&>(c);
    if (m.numberOfRows() != m.numberOfColumns()) {
      return Undefined::Builder();
    }
    int n = m.numberOfRows();
    Addition a = Addition::Builder();
    for (int i = 0; i < n; i++) {
      a.addChildAtIndexInPlace(m.childAtIndex(i+n*i), i, a.numberOfChildren());
    }
    return a.shallowReduce(context, complexFormat, angleUnit);
  }
  if (!c.recursivelyMatches(Expression::IsMatrix)) {
    return c;
  }
  return *this;
#endif
#else
  if (c.type() != ExpressionNode::Type::Matrix) {
    replaceWithInPlace(c);
    return c;
  }
  return *this;
#endif
}

}
