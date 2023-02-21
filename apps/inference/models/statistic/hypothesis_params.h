#ifndef INFERENCE_MODELS_HYPOTHESIS_PARAMS_H
#define INFERENCE_MODELS_HYPOTHESIS_PARAMS_H

#include <poincare/comparison.h>

namespace Inference {

struct HypothesisParams {
 public:
  HypothesisParams()
      : m_firstParam(0.0),
        m_operator(Poincare::ComparisonNode::OperatorType::Superior) {}

  double firstParam() const { return m_firstParam; }
  void setFirstParam(double firstParam) { m_firstParam = firstParam; }

  Poincare::ComparisonNode::OperatorType comparisonOperator() const {
    return m_operator;
  }
  void setComparisonOperator(const Poincare::ComparisonNode::OperatorType op) {
    assert(op == Poincare::ComparisonNode::OperatorType::Inferior ||
           op == Poincare::ComparisonNode::OperatorType::Superior ||
           op == Poincare::ComparisonNode::OperatorType::NotEqual);
    m_operator = op;
  }

 private:
  double m_firstParam;
  Poincare::ComparisonNode::OperatorType m_operator;
};

}  // namespace Inference

#endif
