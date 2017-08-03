#include <poincare/arc_sine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

ArcSine::ArcSine() :
  Function("asin")
{
}

Expression::Type ArcSine::type() const {
  return Type::ArcSine;
}

Expression * ArcSine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ArcSine * s = new ArcSine();
  s->setArgument(newOperands, numberOfOperands, cloneOperands);
  return s;
}

template<typename T>
Complex<T> ArcSine::templatedComputeComplex(const Complex<T> c, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  T result = std::asin(c.a());
  if (angleUnit == AngleUnit::Degree) {
    return Complex<T>::Float(result*180/M_PI);
  }
  return Complex<T>::Float(result);
}

}
