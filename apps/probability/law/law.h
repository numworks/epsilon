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
  virtual ~Law() {};
  virtual const char * title() = 0;
  virtual Type type() const = 0;
  virtual bool isContinuous() const = 0;
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
  virtual float evaluateAtAbscissa(float x) const = 0;
  virtual bool authorizedValueAtIndex(float x, int index) const = 0;
  virtual float cumulativeDistributiveFunctionAtAbscissa(float x) const;
  float rightIntegralFromAbscissa(float x) const;
  float finiteIntegralBetweenAbscissas(float a, float b) const;
  virtual float cumulativeDistributiveInverseForProbability(float * probability);
  float rightIntegralInverseForProbability(float * probability);
protected:
  constexpr static float k_minNumberOfXGridUnits = 7.0f;
  constexpr static float k_maxNumberOfXGridUnits = 18.0f;
  constexpr static float k_oneUnit = 1.0f;
  constexpr static float k_twoUnit = 2.0f;
  constexpr static float k_fiveUnit = 5.0f;
  constexpr static float k_minMarginFactor = -0.2f;
  constexpr static float k_maxMarginFactor = 1.2f;
};

}

#endif
