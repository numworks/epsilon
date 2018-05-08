#ifndef POINCARE_RANDINT_H
#define POINCARE_RANDINT_H

#include <poincare/static_hierarchy.h>
#include <poincare/layout_engine.h>

namespace Poincare {

class Randint : public StaticHierarchy<2> {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  const char * name() const { return "randint"; }
  /* Evaluation */
  Expression * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return templateApproximate<float>(context, angleUnit);
  }
  Expression * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return templateApproximate<double>(context, angleUnit);
  }
  template <typename T> Expression * templateApproximate(Context& context, AngleUnit angleUnit) const;
};

}

#endif
