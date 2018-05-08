#ifndef POINCARE_UNDEFINED_H
#define POINCARE_UNDEFINED_H

#include <poincare/static_hierarchy.h>
#include <poincare/complex.h>

namespace Poincare {

class Undefined : public StaticHierarchy<0> {
public:
  Type type() const override;
  Expression * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override;
  int polynomialDegree(char symbolName) const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override;
  /* Evaluation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Complex<T> * templatedApproximate(Context& context, AngleUnit angleUnit) const;
};

}

#endif
