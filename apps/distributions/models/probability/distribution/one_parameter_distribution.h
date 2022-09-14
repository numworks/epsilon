#ifndef PROBABILITE_ONE_PARAMETER_DISTRIBUTION_H
#define PROBABILITE_ONE_PARAMETER_DISTRIBUTION_H

#include "distribution.h"
#include <assert.h>

namespace Inference {

class OneParameterDistribution : public Distribution {
public:
  OneParameterDistribution(Poincare::Distribution::Type type, double parameterValue) : Distribution(type), m_parameter(parameterValue) {}
  int numberOfParameters() override { return 1; }
protected:
  double * parametersArray() override { return &m_parameter; }
  const double * constParametersArray() const override { return &m_parameter; }
  double m_parameter;
};

}

#endif
