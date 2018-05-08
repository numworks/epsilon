#include <poincare/square_root.h>
#include <poincare/power.h>
#include <poincare/simplification_engine.h>
#include "layout/nth_root_layout.h"
extern "C" {
#include <assert.h>
}
#include <cmath>
#include <ion.h>

namespace Poincare {

Expression::Type SquareRoot::type() const {
  return Type::SquareRoot;
}

Expression * SquareRoot::clone() const {
  SquareRoot * a = new SquareRoot(m_operands, true);
  return a;
}

static_assert('\x90' == Ion::Charset::Root, "Unicode error");
int SquareRoot::writeTextInBuffer(char * buffer, int bufferSize, PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "\x90");
}

template<typename T>
std::complex<T> SquareRoot::computeOnComplex(const std::complex<T> c, AngleUnit angleUnit) {
  std::complex<T> result = std::sqrt(c);
  /* Openbsd trigonometric functions are numerical implementation and thus are
   * approximative.
   * The error epsilon is ~1E-7 on float and ~1E-15 on double. In order to
   * avoid weird results as sqrt(-1) = 6E-16+i, we compute the argument of
   * the result of sqrt(c) and if arg ~ 0 [Pi], we discard the residual imaginary
   * part and if arg ~ Pi/2 [Pi], we discard the residual real part. */
  return ApproximationEngine::truncateRealOrImaginaryPartAccordingToArgument(result);
}

Expression * SquareRoot::shallowReduce(Context& context, AngleUnit angleUnit) {
  Expression * e = Expression::shallowReduce(context, angleUnit);
  if (e != this) {
    return e;
  }
#if MATRIX_EXACT_REDUCING
  if (operand(0)->type() == Type::Matrix) {
    return SimplificationEngine::map(this, context, angleUnit);
  }
#endif
  Power * p = new Power(operand(0), new Rational(1, 2), false);
  detachOperands();
  replaceWith(p, true);
  return p->shallowReduce(context, angleUnit);
}

ExpressionLayout * SquareRoot::createLayout(PrintFloat::Mode floatDisplayMode, int numberOfSignificantDigits) const {
  return new NthRootLayout(operand(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), false);
}

}
