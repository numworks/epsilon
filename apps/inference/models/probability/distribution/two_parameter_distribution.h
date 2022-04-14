#ifndef PROBABILITE_TWO_PARAMETER_DISTRIBUTION_H
#define PROBABILITE_TWO_PARAMETER_DISTRIBUTION_H

#include "distribution.h"

namespace Inference {

class TwoParameterDistribution : public Distribution {
public:
  TwoParameterDistribution(double parameterValue1, double parameterValue2) :
    m_parameters{parameterValue1, parameterValue2}
  {}
  int numberOfParameters() override { return 2; }
protected:
  double * parametersArray() override { return m_parameters; }
  double m_parameters[2];
};

}

#endif
