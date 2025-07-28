#ifndef PROBABILITE_ONE_PARAMETER_DISTRIBUTION_H
#define PROBABILITE_ONE_PARAMETER_DISTRIBUTION_H

#include <assert.h>

#include "distribution.h"

namespace Distributions {

class OneParameterDistribution : public Distribution {
 public:
  OneParameterDistribution(Poincare::Distribution::Type type)
      : Distribution(type),
        m_parameter(Poincare::Distribution::DefaultParameterAtIndex(type, 0)) {}

 protected:
  double* parametersArray() override { return &m_parameter; }
  double m_parameter;
};

}  // namespace Distributions

#endif
