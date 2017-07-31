#include <poincare/arc_sine.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

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

Complex ArcSine::computeComplex(const Complex c, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  float result = asinf(c.a());
  if (angleUnit == AngleUnit::Degree) {
    return Complex::Float(result*180.0f/M_PI);
  }
  return Complex::Float(result);
}

}
