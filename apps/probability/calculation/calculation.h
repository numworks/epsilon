#ifndef PROBABILITE_CALCULATION_H
#define PROBABILITE_CALCULATION_H

#include "../law/law.h"

namespace Probability {

class Calculation {
public:
  Calculation();
  virtual ~Calculation() {};
  void setLaw(Law * law);
  virtual int numberOfParameters() = 0;
  virtual const char * legendForParameterAtIndex(int index) = 0;
  virtual void setParameterAtIndex(float f, int index) = 0;
  virtual float parameterAtIndex(int index) = 0;
  virtual float lowerBound();
  virtual float upperBound();
protected:
  virtual void compute(int indexKnownElement) = 0;
  Law * m_law;
};

}

#endif
