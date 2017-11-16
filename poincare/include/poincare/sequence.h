#ifndef POINCARE_SEQUENCE_H
#define POINCARE_SEQUENCE_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>

namespace Poincare {

class Sequence : public StaticHierarchy<3>  {
  using StaticHierarchy<3>::StaticHierarchy;
private:
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, name());
  }
  virtual ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const = 0;
  virtual const char * name() const = 0;
  /* Evaluation */
  Expression * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Expression * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
 template<typename T> Expression * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
  virtual int emptySequenceValue() const = 0;
  virtual Expression * evaluateWithNextTerm(SinglePrecision p, Expression * a, Expression * b) const = 0;
  virtual Expression * evaluateWithNextTerm(DoublePrecision p, Expression * a, Expression * b) const = 0;
};

}

#endif
