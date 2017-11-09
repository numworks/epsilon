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
  ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const override;
  Complex<float> * evaluateWithNextTerm(Complex<float> * a, Complex<float> * b) const override {
    return templatedEvaluateWithNextTerm(a, b);
  }
  Complex<double> * evaluateWithNextTerm(Complex<double> * a, Complex<double> * b) const override {
    return templatedEvaluateWithNextTerm(a, b);
  }
  template<typename T> Complex<T> * templatedEvaluateWithNextTerm(Complex<T> * a, Complex<T> * b) const;
};

}

#endif
