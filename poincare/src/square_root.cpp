#include <poincare/square_root.h>
#include <poincare/power.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/simplification_helper.h>
#include <poincare/nth_root_layout.h>
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

constexpr Expression::FunctionHelper SquareRoot::s_functionHelper;

int SquareRootNode::numberOfChildren() const { return SquareRoot::s_functionHelper.numberOfChildren(); }

Layout SquareRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return NthRootLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int SquareRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SquareRoot::s_functionHelper.name());
}

template<typename T>
Complex<T> SquareRootNode::computeOnComplex(const std::complex<T> c, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::sqrt(c);
  /* Openbsd trigonometric functions are numerical implementation and thus are
   * approximative.
   * The error epsilon is ~1E-7 on float and ~1E-15 on double. In order to avoid
   * weird results as sqrt(-1) = 6E-16+i, we compute the argument of the result
   * of sqrt(c) and if arg ~ 0 [Pi], we discard the residual imaginary part and
   * if arg ~ Pi/2 [Pi], we discard the residual real part.*/
  return Complex<T>(ApproximationHelper::TruncateRealOrImaginaryPartAccordingToArgument(result));
}

Expression SquareRootNode::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  return SquareRoot(this).shallowReduce(context, angleUnit, replaceSymbols);
}

Expression SquareRoot::shallowReduce(Context & context, Preferences::AngleUnit angleUnit, bool replaceSymbols) {
  {
    Expression e = Expression::defaultShallowReduce(context, angleUnit);
    if (e.isUndefined()) {
      return e;
    }
  }
#if MATRIX_EXACT_REDUCING
  if (childAtIndex(0).type() == ExpressionNode::Type::Matrix) {
    return SimplificationHelper::Map(this, context, angleUnit);
  }
#endif
  Power p = Power(childAtIndex(0), Rational(1, 2));
  replaceWithInPlace(p);
  return p.shallowReduce(context, angleUnit);
}

}
