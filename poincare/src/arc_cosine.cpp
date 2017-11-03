#include <poincare/arc_cosine.h>
#include <poincare/trigonometry.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Expression::Type ArcCosine::type() const {
  return Type::ArcCosine;
}

Expression * ArcCosine::clone() const {
  ArcCosine * a = new ArcCosine(m_operands, true);
  return a;
}

Expression * ArcCosine::shallowReduce(Context& context, AngleUnit angleUnit) {
  return Trigonometry::shallowReduceInverseFunction(this, context, angleUnit);
}

template<typename T>
Complex<T> ArcCosine::computeOnComplex(const Complex<T> c, AngleUnit angleUnit) {
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
