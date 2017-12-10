#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/sequence.h>

namespace Poincare {

class Product final : public Sequence {
  using Sequence::Sequence;
public:
  Type type() const override;
  Expression * clone() const override {
    return new Product(m_operands, true);
  }
private:
  const char * name() const override { return "product"; }
  int emptySequenceValue() const override { return 1; }
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
