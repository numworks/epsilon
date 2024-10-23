#ifndef INFERENCE_MODELS_HYPOTHESIS_PARAMS_H
#define INFERENCE_MODELS_HYPOTHESIS_PARAMS_H

#include <poincare/comparison_operator.h>

namespace Inference {

struct HypothesisParams {
 public:
  HypothesisParams()
      : m_firstParam(0.0),
        m_operator(Poincare::ComparisonJunior::Operator::Superior) {}

  double firstParam() const { return m_firstParam; }
  void setFirstParam(double firstParam) { m_firstParam = firstParam; }

  Poincare::ComparisonJunior::Operator comparisonOperator() const {
    return m_operator;
  }
  void setComparisonOperator(const Poincare::ComparisonJunior::Operator op) {
    assert(op == Poincare::ComparisonJunior::Operator::Inferior ||
           op == Poincare::ComparisonJunior::Operator::Superior ||
           op == Poincare::ComparisonJunior::Operator::NotEqual);
    m_operator = op;
  }

 private:
  double m_firstParam;
  Poincare::ComparisonJunior::Operator m_operator;
};

}  // namespace Inference

#endif
