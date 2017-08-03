#include <poincare/arc_cosine.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

ArcCosine::ArcCosine() :
  Function("acos")
{
}

Expression::Type ArcCosine::type() const {
  return Type::ArcCosine;
}

Expression * ArcCosine::cloneWithDifferentOperands(Expression** newOperands,
        int numberOfOperands, bool cloneOperands) const {
  assert(newOperands != nullptr);
  ArcCosine * c = new ArcCosine();
  c->setArgument(newOperands, numberOfOperands, cloneOperands);
  return c;
}

template<typename T>
Complex<T> ArcCosine::templatedComputeComplex(const Complex<T> c, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() != 0) {
    return Complex<T>::Float(NAN);
  }
  T result = std::acos(c.a());
  if (angleUnit == AngleUnit::Degree) {
    return Complex<T>::Float(result*180/M_PI);
  }
  return Complex<T>::Float(result);
}

}
