#ifndef PROBABILITY_MODELS_CALCULATION_CALCULATION_H
#define PROBABILITY_MODELS_CALCULATION_CALCULATION_H

#include <apps/i18n.h>
#include <assert.h>
#include <stdint.h>

namespace Probability {

class Distribution;

class Calculation {
public:
  enum class Type : uint8_t{
    LeftIntegral,
    FiniteIntegral,
    RightIntegral,
    Discrete,
  };
  static void Initialize(Calculation * calculation, Type type, Distribution * distribution);

  Calculation(Distribution * distribution) : m_distribution(distribution) {
    assert(distribution != nullptr);
  }
  virtual ~Calculation() = default;
  virtual Type type() = 0;
  virtual int numberOfParameters() = 0;
  virtual I18n::Message legendForParameterAtIndex(int index) = 0;
  virtual void setParameterAtIndex(double f, int index) = 0;
  virtual double parameterAtIndex(int index) = 0;
  virtual double lowerBound() const;
  virtual double upperBound() const;
protected:
  virtual void compute(int indexKnownElement) = 0;
  Distribution * m_distribution;
};

}

#endif /* PROBABILITY_MODELS_CALCULATION_CALCULATION_H */
