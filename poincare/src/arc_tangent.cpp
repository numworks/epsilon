#include <poincare/arc_tangent.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

ArcTangent::ArcTangent() :
  Function("atan")
{
}

Expression::Type ArcTangent::type() const {
  return Type::ArcTangent;
}

Expression * ArcTangent::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ArcTangent * t = new ArcTangent();
  t->setArgument(newOperands, numberOfOperands, cloneOperands);
  return t;
}

template<typename T>
Complex<T> ArcTangent::templatedComputeComplex(const Complex<T> c, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  T result = std::atan(c.a());
  if (angleUnit == AngleUnit::Degree) {
    return Complex<T>::Float(result*180/M_PI);
  }
  return Complex<T>::Float(result);
}

}
