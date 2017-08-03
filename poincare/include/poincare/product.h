#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/sequence.h>

namespace Poincare {

class Product : public Sequence {
public:
  Product();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  float emptySequenceValue() const override;
  ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const override;
  Evaluation * evaluateWithNextTerm(Evaluation * a, Evaluation * b) const override;
};

}

#endif
