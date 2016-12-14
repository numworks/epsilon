#ifndef PROBABILITE_CALCULATION_H
#define PROBABILITE_CALCULATION_H

#include "evaluate_context.h"
#include "law.h"

namespace Probability {

class Calculation {
public:
  enum Type : uint8_t {
    LeftIntegral = 0,
    FiniteIntegral = 1,
    RightIntegral = 2,
  };
  Calculation();
  void setLaw(Law * law);
  void setType(Type type);
  Type type() const;
  float parameterAtIndex(int index);
  void setParameterAtIndex(float f, int index);
private:
  void computeCalculation(int indexKnownElement);
  void initParameters();
  Type m_type;
  float m_parameter1;
  float m_parameter2;
  float m_parameter3;
  Law * m_law;
};

}

#endif
