#ifndef APPS_PROBABILITY_MODELS_CHI2_LAW_H
#define APPS_PROBABILITY_MODELS_CHI2_LAW_H

#include <float.h>

namespace Probability {

class Chi2Law {
public:
  template <typename T>
  static T EvaluateAtAbscissa(T x, T k);
  template <typename T>
  static T CumulativeDistributiveFunctionAtAbscissa(T x, T k);
  template <typename T>
  static T CumulativeDistributiveInverseForProbability(T probability, T k);

private:
  static constexpr int k_maxRegularizedGammaIterations = 1000;
  static constexpr double k_regularizedGammaPrecision = DBL_EPSILON;
};

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_CHI2_LAW_H */
