#include <poincare/square_root.h>
#include <poincare/complex.h>
#include <poincare/power.h>
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
int SquareRoot::writeTextInBuffer(char * buffer, int bufferSize) const {
  return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, "\x90");
}

template<typename T>
Complex<T> SquareRoot::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
  if (c.b() == 0 && c.a() >= 0) {
    return Complex<T>::Float(std::sqrt(c.a()));
  }
  return Power::compute(c, Complex<T>::Float(0.5));
}

Expression * SquareRoot::immediateSimplify(Context& context, AngleUnit angleUnit) {
  const Expression * powOperands[2] = {operand(0), new Rational(Integer(1), Integer(2))};
  Power * p = new Power(powOperands, false);
  detachOperands();
  replaceWith(p, true);
  return p->immediateSimplify(context, angleUnit);
}

ExpressionLayout * SquareRoot::privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  assert(complexFormat != ComplexFormat::Default);
  return new NthRootLayout(operand(0)->createLayout(floatDisplayMode, complexFormat),nullptr);
}

}
