#ifndef PROBABILITE_CALCULATION_H
#define PROBABILITE_CALCULATION_H

#include "../law/law.h"

namespace Probability {

class Calculation {
public:
  enum class Type : uint8_t{
    LeftIntegral,
    FiniteIntegral,
    RightIntegral,
    Discrete,
  };
  Calculation();
  virtual ~Calculation() = default;
  virtual Type type() = 0;
  void setLaw(Law * law);
  virtual int numberOfParameters() = 0;
  virtual int numberOfEditableParameters();
  virtual I18n::Message legendForParameterAtIndex(int index) = 0;
  virtual void setParameterAtIndex(double f, int index) = 0;
  virtual double parameterAtIndex(int index) = 0;
  virtual double lowerBound();
  virtual double upperBound();
protected:
  virtual void compute(int indexKnownElement) = 0;
  Law * m_law;
};

}

#endif
