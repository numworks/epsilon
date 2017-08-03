#include <poincare/hyperbolic_arc_sine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

HyperbolicArcSine::HyperbolicArcSine() :
  Function("asinh")
{
}

Expression::Type HyperbolicArcSine::type() const {
  return Type::HyperbolicArcSine;
}

Expression * HyperbolicArcSine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  HyperbolicArcSine * s = new HyperbolicArcSine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

template<typename T>
Complex<T> HyperbolicArcSine::templatedComputeComplex(const Complex<T> c) const {
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  return Complex<T>::Float(std::asinh(c.a()));
}

}
