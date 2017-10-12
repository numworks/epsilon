#ifndef POINCARE_LOGARITHM_H
#define POINCARE_LOGARITHM_H

#include <poincare/layout_engine.h>
#include <poincare/bounded_static_hierarchy.h>
#include <poincare/evaluation_engine.h>
#include <poincare/integer.h>

namespace Poincare {

class Logarithm : public BoundedStaticHierarchy<2>  {
  using BoundedStaticHierarchy<2>::BoundedStaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
  Expression * immediateSimplify() override;
private:
  template<typename T> static Complex<T> computeOnComplex(const Complex<T> c, AngleUnit angleUnit);
  Evaluation<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Evaluation<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  /* Simplification */
  Expression * splitInteger(Integer i, bool isDenominator);
  Expression * immediateBeautify() override;
};

}

#endif
