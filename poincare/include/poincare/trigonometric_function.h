#ifndef POINCARE_TRIGONOMETRIC_FUNCTION_H
#define POINCARE_TRIGONOMETRIC_FUNCTION_H

#include <poincare/function.h>

namespace Poincare {

class TrigonometricFunction : public Function {
public:
  TrigonometricFunction(const char * name);
private:
  Complex computeComplex(const Complex c, AngleUnit angleUnit) const override;
  virtual float computeForRadianReal(float x) const = 0;
  virtual Complex privateCompute(const Complex c, AngleUnit angleUnit) const = 0;
};

}

#endif
