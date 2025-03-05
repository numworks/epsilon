#ifndef PROBABILITE_TWO_PARAMETERS_DISTRIBUTION_H
#define PROBABILITE_TWO_PARAMETERS_DISTRIBUTION_H

#include "distribution.h"
#include "poincare/statistics/distribution.h"

namespace Distributions {

class TwoParametersDistribution : public Distribution {
 public:
  TwoParametersDistribution(Poincare::Distribution::Type type)
      : Distribution(type),
        m_parameters{Poincare::Distribution::DefaultParameterAtIndex(type, 0),
                     Poincare::Distribution::DefaultParameterAtIndex(type, 1)} {
  }

 protected:
  double* parametersArray() override { return m_parameters; }
  double m_parameters[2];
};

}  // namespace Distributions

#endif
