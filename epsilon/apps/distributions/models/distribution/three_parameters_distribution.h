#ifndef PROBABILITE_THREE_PARAMETERS_DISTRIBUTION_H
#define PROBABILITE_THREE_PARAMETERS_DISTRIBUTION_H

#include "distribution.h"

namespace Distributions {

class ThreeParametersDistribution : public Distribution {
 public:
  ThreeParametersDistribution(Poincare::Distribution::Type type)
      : Distribution(type),
        m_parameters{Poincare::Distribution::DefaultParameterAtIndex(type, 0),
                     Poincare::Distribution::DefaultParameterAtIndex(type, 1),
                     Poincare::Distribution::DefaultParameterAtIndex(type, 2)} {
  }

 protected:
  double* parametersArray() override { return m_parameters; }
  double m_parameters[3];
};

}  // namespace Distributions

#endif
