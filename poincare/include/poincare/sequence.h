#ifndef POINCARE_SEQUENCE_H
#define POINCARE_SEQUENCE_H

#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class SequenceNode : public ExpressionNode {
public:
  int numberOfChildren() const override { return 3; }
private:
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, name());
  }
  virtual Layout createSequenceLayout(Layout subscriptLayout, Layout superscriptLayout, Layout argumentLayout) const = 0;
  virtual const char * name() const = 0;
  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
  virtual float emptySequenceValue() const = 0;
  virtual Evaluation<float> evaluateWithNextTerm(SinglePrecision p, Evaluation<float> a, Evaluation<float> b) const = 0;
  virtual Evaluation<double> evaluateWithNextTerm(DoublePrecision p, Evaluation<double> a, Evaluation<double> b) const = 0;
};

}

#endif
