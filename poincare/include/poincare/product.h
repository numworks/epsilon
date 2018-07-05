#ifndef POINCARE_PRODUCT_H
#define POINCARE_PRODUCT_H

#include <poincare/sequence.h>

namespace Poincare {

class Product : public Sequence {
  using Sequence::Sequence;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  const char * name() const override;
  int emptySequenceValue() const override;
  ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * argumentLayout, ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout) const override;
  Evaluation<double> * evaluateWithNextTerm(DoublePrecision p, Evaluation<double> * a, Evaluation<double> * b) const override {
    return templatedApproximateWithNextTerm<double>(a, b);
  }
  Evaluation<float> * evaluateWithNextTerm(SinglePrecision p, Evaluation<float> * a, Evaluation<float> * b) const override {
    return templatedApproximateWithNextTerm<float>(a, b);
  }
  template<typename T> Evaluation<T> * templatedApproximateWithNextTerm(Evaluation<T> * a, Evaluation<T> * b) const;
};

}

#endif
