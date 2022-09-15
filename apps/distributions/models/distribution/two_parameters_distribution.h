#ifndef PROBABILITE_TWO_PARAMETERS_DISTRIBUTION_H
#define PROBABILITE_TWO_PARAMETERS_DISTRIBUTION_H

#include "distribution.h"

namespace Distributions {

class TwoParametersDistribution : public Distribution {
public:
  TwoParametersDistribution(Poincare::Distribution::Type type, double parameterValue1, double parameterValue2) :
    Distribution(type),
    m_parameters{parameterValue1, parameterValue2}
  {}
  int numberOfParameters() override { return 2; }
protected:
  double * parametersArray() override { return m_parameters; }
  const double * constParametersArray() const override { return m_parameters; }
  double m_parameters[2];
};

}

#endif
