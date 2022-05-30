#ifndef INFERENCE_MODELS_HYPOTHESIS_PARAMS_H
#define INFERENCE_MODELS_HYPOTHESIS_PARAMS_H

namespace Inference {

struct HypothesisParams {
public:
  enum class ComparisonOperator : char { Lower, Different, Higher };
  HypothesisParams() : m_firstParam(0.0), m_operator(ComparisonOperator::Higher) {}

  double firstParam() const { return m_firstParam; }
  void setFirstParam(double firstParam) { m_firstParam = firstParam; }

  ComparisonOperator comparisonOperator() const { return m_operator; }
  void setComparisonOperator(const ComparisonOperator op) { m_operator = op; }

  static const char * strForComparisonOp(ComparisonOperator op) {
    switch (op) {
      case ComparisonOperator::Lower:
        return "<";
      case ComparisonOperator::Higher:
        return ">";
      case ComparisonOperator::Different:
        return "≠";  // The character is NFKD normalized
    }
    return 0;
  }

private:
  double m_firstParam;
  ComparisonOperator m_operator;
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_HYPOTHESIS_PARAMS_H */
