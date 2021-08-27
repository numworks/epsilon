#ifndef APPS_PROBABILITY_MODELS_HYPOTHESIS_PARAMS_H
#define APPS_PROBABILITY_MODELS_HYPOTHESIS_PARAMS_H

namespace Probability {

struct HypothesisParams {
public:
  enum class ComparisonOperator : char { Lower, Different, Higher };
  HypothesisParams() : m_firstParam(0) {}

  float firstParam() const { return m_firstParam; }
  void setFirstParam(float firstParam) { m_firstParam = firstParam; }

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
  float m_firstParam;
  ComparisonOperator m_operator;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_HYPOTHESIS_PARAMS_H */
