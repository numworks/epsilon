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
  float approximateWithNextTerm(float sequence, float newTerm) const override;
  ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const override;
  Expression * evaluateWithNextTerm(Expression ** args, Context& context, AngleUnit angleUnit) const override;
};

}

#endif
