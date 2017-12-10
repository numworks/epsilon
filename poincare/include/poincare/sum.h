#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include <poincare/sequence.h>

namespace Poincare {

class Sum final : public Sequence {
  using Sequence::Sequence;
public:
  Type type() const override;
  Expression * clone() const override {
    return new Sum(m_operands, true);
  }
private:
  const char * name() const override { return "sum"; }
  int emptySequenceValue() const override { return 0; }
  ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * argumentLayout, ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout) const override;
  Expression * evaluateWithNextTerm(DoublePrecision p, Expression * a, Expression * b) const override {
    return templatedApproximateWithNextTerm<double>(a, b);
  }
  Expression * evaluateWithNextTerm(SinglePrecision p, Expression * a, Expression * b) const override {
    return templatedApproximateWithNextTerm<float>(a, b);
  }
  template<typename T> Expression * templatedApproximateWithNextTerm(Expression * a, Expression * b) const;
};

}

#endif
