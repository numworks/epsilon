#ifndef POINCARE_EMPTY_EXPRESSION_H
#define POINCARE_EMPTY_EXPRESSION_H

#include <poincare/static_hierarchy.h>
#include <poincare/evaluation.h>

namespace Poincare {

/* An empty expression awaits completion by the user. */

class EmptyExpression : public StaticHierarchy<0> {
public:
  Type type() const override {
    return Type::EmptyExpression;
  }
  Expression * clone() const override;
  int writeTextInBuffer(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
private:
  /* Layout */
  LayoutRef createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Complex<T> * templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

}

#endif

