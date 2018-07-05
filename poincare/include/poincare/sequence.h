#ifndef POINCARE_SEQUENCE_H
#define POINCARE_SEQUENCE_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/approximation_engine.h>

namespace Poincare {

class Sequence : public StaticHierarchy<3>  {
  using StaticHierarchy<3>::StaticHierarchy;
private:
  ExpressionLayout * privateCreateLayout(PrintFloat::Mode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize, int numberOfSignificantDigits = PrintFloat::k_numberOfStoredSignificantDigits) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, numberOfSignificantDigits, name());
  }
  virtual ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const = 0;
  virtual const char * name() const = 0;
  /* Evaluation */
  Evaluation<float> * privateApproximate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> * privateApproximate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
 template<typename T> Evaluation<T> * templatedApproximate(Context& context, AngleUnit angleUnit) const;
  virtual int emptySequenceValue() const = 0;
  virtual Evaluation<float> * evaluateWithNextTerm(SinglePrecision p, Evaluation<float> * a, Evaluation<float> * b) const = 0;
  virtual Evaluation<double> * evaluateWithNextTerm(DoublePrecision p, Evaluation<double> * a, Evaluation<double> * b) const = 0;
};

}

#endif
