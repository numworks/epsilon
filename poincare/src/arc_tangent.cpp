#include <poincare/arc_tangent.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

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

Complex ArcTangent::computeComplex(const Complex c, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  float result = atanf(c.a());
  if (angleUnit == AngleUnit::Degree) {
    return Complex::Float(result*180.0f/M_PI);
  }
  return  Complex::Float(result);
}

}
