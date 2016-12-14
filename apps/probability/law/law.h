#ifndef PROBABILITE_LAW_H
#define PROBABILITE_LAW_H

#include <poincare.h>

namespace Probability {

class Law {
public:
  enum class Type : uint8_t{
    Binomial,
    Uniform,
    Exponential,
    Normal,
    Poisson
  };
  Law(Context * context);
  virtual ~Law() {};
  virtual const char * title() = 0;
  Context * context();
  virtual Type type() const = 0;
  virtual Expression * expression() const = 0;
  virtual bool isContinuous() = 0;
  virtual float xMin() = 0;
  virtual float yMin() = 0;
  virtual float xMax() = 0;
  virtual float yMax() = 0;
  float gridUnit();
  virtual int numberOfParameter() = 0;
  virtual float parameterValueAtIndex(int index) = 0;
  virtual const char * parameterNameAtIndex(int index) = 0;
  virtual const char * parameterDefinitionAtIndex(int index) = 0;
  virtual void setParameterAtIndex(float f, int index) = 0;
  float evaluateAtAbscissa(float x) const;
protected:
  constexpr static float k_minNumberOfXGridUnits = 7.0f;
  constexpr static float k_maxNumberOfXGridUnits = 18.0f;
  constexpr static float k_oneUnit = 1.0f;
  constexpr static float k_twoUnit = 2.0f;
  constexpr static float k_fiveUnit = 5.0f;
  Context * m_context;
};

}

#endif
