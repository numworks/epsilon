#include <poincare/trigonometric_function.h>
#include <poincare/complex.h>
extern "C" {
#include <assert.h>
#include <math.h>
}

namespace Poincare {

TrigonometricFunction::TrigonometricFunction(const char * name) :
  Function(name)
{
}

Complex TrigonometricFunction::computeComplex(const Complex c, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (c.b() == 0.0f) {
    float input = c.a();
    if (angleUnit == AngleUnit::Degree) {
      input *= M_PI/180.0f;
    }
    float result = computeForRadianReal(input);
    if (input !=  0.0f && fabsf(result/input) <= 1E-7f) {
      return Complex::Float(0.0f);
    }
    return Complex::Float(result);
  }
  return privateCompute(c, angleUnit);
}

}
