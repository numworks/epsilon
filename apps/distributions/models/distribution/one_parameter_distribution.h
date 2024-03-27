#ifndef PROBABILITE_ONE_PARAMETER_DISTRIBUTION_H
#define PROBABILITE_ONE_PARAMETER_DISTRIBUTION_H

#include <assert.h>

#include "distribution.h"

namespace Distributions {

class OneParameterDistribution : public Distribution {
 public:
  OneParameterDistribution(PoincareJ::Distribution::Type type,
                           double parameterValue)
      : Distribution(type), m_parameter(parameterValue) {}
  int numberOfParameters() override { return 1; }
  const double* constParametersArray() const override { return &m_parameter; }

 protected:
  double* parametersArray() override { return &m_parameter; }
  double m_parameter;
};

}  // namespace Distributions

#endif
