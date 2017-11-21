#ifndef POINCARE_DIVISION_QUOTIENT_H
#define POINCARE_DIVISION_QUOTIENT_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/complex.h>

namespace Poincare {

class DivisionQuotient : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
private:
 /* Layout */
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, name());
  }
  const char * name() const { return "quo"; }
  /* Simplification */
  Expression * shallowReduce(Context & context, AngleUnit angleUnit) override;
  /* Evaluation */
  Expression * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Expression * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Complex<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
};

}

#endif

