#ifndef PROBABILITE_THREE_PARAMETERS_DISTRIBUTION_H
#define PROBABILITE_THREE_PARAMETERS_DISTRIBUTION_H

#include "distribution.h"

namespace Distributions {

class ThreeParametersDistribution : public Distribution {
 public:
  ThreeParametersDistribution(Poincare::Distribution::Type type,
                              double parameterValue1, double parameterValue2,
                              double parameterValue3)
      : Distribution(type),
        m_parameters{parameterValue1, parameterValue2, parameterValue3} {}
  int numberOfParameters() override { return 3; }

 protected:
  double* parametersArray() override { return m_parameters; }
  const double* constParametersArray() const override { return m_parameters; }
  double m_parameters[3];
};

}  // namespace Distributions

#endif
