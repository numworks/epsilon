#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include <poincare/sequence.h>

namespace Poincare {

class Sum : public Sequence {
  using Sequence::Sequence;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  const char * name() const override;
  int emptySequenceValue() const override;
  ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const override;
  Expression * evaluateWithNextTerm(DoublePrecision p, Expression * a, Expression * b) const override {
    return templatedEvaluateWithNextTerm<double>(a, b);
  }
  Expression * evaluateWithNextTerm(SinglePrecision p, Expression * a, Expression * b) const override {
    return templatedEvaluateWithNextTerm<float>(a, b);
  }
  template<typename T> Expression * templatedEvaluateWithNextTerm(Expression * a, Expression * b) const;
};

}

#endif
