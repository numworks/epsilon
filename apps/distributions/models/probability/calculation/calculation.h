#ifndef DISTRIBUTIONS_MODELS_CALCULATION_CALCULATION_H
#define DISTRIBUTIONS_MODELS_CALCULATION_CALCULATION_H

#include <apps/i18n.h>
#include <assert.h>
#include <stdint.h>
#include <new>

namespace Distributions {

class Distribution;

class Calculation {
friend union DistributionBuffer;
public:
  enum class Type : uint8_t{
    LeftIntegral,
    FiniteIntegral,
    RightIntegral,
    Discrete,
  };
  static bool Initialize(Calculation * calculation, Type type, Distribution * distribution, bool forceReinitialization);

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

#endif /* DISTRIBUTIONS_MODELS_CALCULATION_CALCULATION_H */
