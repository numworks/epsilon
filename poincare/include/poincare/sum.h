#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include <poincare/sequence.h>

namespace Poincare {

class Sum : public Sequence {
public:
  Sum();
  Type type() const override;
  Expression * cloneWithDifferentOperands(Expression ** newOperands,
      int numberOfOperands, bool cloneOperands = true) const override;
private:
  int emptySequenceValue() const override;
  ExpressionLayout * createSequenceLayoutWithArgumentLayouts(ExpressionLayout * subscriptLayout, ExpressionLayout * superscriptLayout, ExpressionLayout * argumentLayout) const override;
  Evaluation<float> * evaluateWithNextTerm(Evaluation<float> * a, Evaluation<float> * b) const override {
    return templatedEvaluateWithNextTerm(a, b);
  }
  Evaluation<double> * evaluateWithNextTerm(Evaluation<double> * a, Evaluation<double> * b) const override {
    return templatedEvaluateWithNextTerm(a, b);
  }
  template<typename T> Evaluation<T> * templatedEvaluateWithNextTerm(Evaluation<T> * a, Evaluation<T> * b) const;
};

}

#endif
