#ifndef POINCARE_SUM_H
#define POINCARE_SUM_H

#include <poincare/sequence.h>

namespace Poincare {

class Sum : public Sequence {
  using Sequence::Sequence;
public:
  Type type() const override;
private:
  const char * name() const override;
  int emptySequenceValue() const override;
  LayoutRef createSequenceLayout(LayoutRef argumentLayout, LayoutRef subscriptLayout, LayoutRef superscriptLayout) const override;
  EvaluationReference<double> evaluateWithNextTerm(DoublePrecision p, Evaluation<double> * a, Evaluation<double> * b) const override {
    return templatedApproximateWithNextTerm<double>(a, b);
  }
  EvaluationReference<float> evaluateWithNextTerm(SinglePrecision p, Evaluation<float> * a, Evaluation<float> * b) const override {
    return templatedApproximateWithNextTerm<float>(a, b);
  }
  template<typename T> EvaluationReference<T> templatedApproximateWithNextTerm(Evaluation<T> * a, Evaluation<T> * b) const;
};

}

#endif
