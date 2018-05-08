#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/layout_engine.h>
#include <poincare/bounded_static_hierarchy.h>
#include <poincare/integer.h>

namespace Poincare {

class Logarithm : public BoundedStaticHierarchy<2>  {
  using BoundedStaticHierarchy<2>::BoundedStaticHierarchy;
  friend class NaperianLogarithm;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, "log");
  }
  /* Simplification */
  Expression * shallowReduce(Context & context, AngleUnit angleUnit) override;
  Expression * simpleShallowReduce(Context & context, AngleUnit angleUnit);
  Expression * shallowBeautify(Context & context, AngleUnit angleUnit) override;
  bool parentIsAPowerOfSameBase() const;
  Expression * splitInteger(Integer i, bool isDenominator, Context & context, AngleUnit angleUnit);
  /* Evaluation */
  template<typename T> static Complex<T> computeOnComplex(const Complex<T> c, AngleUnit angleUnit);
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Expression * templatedApproximate(Context& context, AngleUnit angleUnit) const;
};

}

#endif
