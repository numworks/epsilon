#ifndef PROBABILITY_MODELS_STUDENT_LAW_H
#define PROBABILITY_MODELS_STUDENT_LAW_H

namespace Probability {

class StudentLaw final {
public:
  template <typename T>
  static T EvaluateAtAbscissa(T x, T k);
  template <typename T>
  static T CumulativeDistributiveFunctionAtAbscissa(T x, T k);
  template <typename T>
  static T CumulativeDistributiveInverseForProbability(T probability, T k);
  template <typename T>
  static T lnCoefficient(T k);
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STUDENT_LAW_H */
