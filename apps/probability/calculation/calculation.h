#ifndef PROBABILITE_CALCULATION_H
#define PROBABILITE_CALCULATION_H

#include "../distribution/distribution.h"

namespace Probability {

class Calculation {
public:
  enum class Type : uint8_t{
    LeftIntegral,
    FiniteIntegral,
    RightIntegral,
    Discrete,
  };
  Calculation(Distribution * distribution) : m_distribution(distribution) {
    assert(distribution != nullptr);
  }
  virtual ~Calculation() = default;
  virtual Type type() = 0;
  virtual int numberOfParameters() = 0;
  virtual I18n::Message legendForParameterAtIndex(int index) = 0;
  virtual void setParameterAtIndex(double f, int index) = 0;
  virtual double parameterAtIndex(int index) = 0;
  virtual double lowerBound();
  virtual double upperBound();
protected:
  virtual void compute(int indexKnownElement) = 0;
  Distribution * m_distribution;
};

}

#endif
