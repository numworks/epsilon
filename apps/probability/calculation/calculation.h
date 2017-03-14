#ifndef PROBABILITE_CALCULATION_H
#define PROBABILITE_CALCULATION_H

#include "../law/law.h"

namespace Probability {

class Calculation {
public:
  enum class Type : uint8_t{
    LeftIntegral,
    RightIntegral,
    FiniteIntegral
  };
  Calculation();
  virtual ~Calculation() {};
  virtual Type type() = 0;
  void setLaw(Law * law);
  virtual int numberOfParameters() = 0;
  virtual I18n::Message legendForParameterAtIndex(int index) = 0;
  virtual void setParameterAtIndex(float f, int index) = 0;
  virtual float parameterAtIndex(int index) = 0;
  virtual float lowerBound();
  virtual float upperBound();
protected:
  /* Parameters in probability application are rounded to 3 decimals. This is
   * due to the limited precision of some calculation (e. g. standard normal
   * cumulative distributive function or inverse). */
  constexpr static float k_precision = 0.001f;
  virtual void compute(int indexKnownElement) = 0;
  Law * m_law;
};

}

#endif
