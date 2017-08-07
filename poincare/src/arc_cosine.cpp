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

Complex ArcCosine::computeComplex(const Complex c, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() != 0.0f) {
    return Complex::Float(NAN);
  }
  float result = std::acos(c.a());
  if (angleUnit == AngleUnit::Degree) {
    return Complex::Float(result*180.0f/M_PI);
  }
  return Complex::Float(result);
}

}
