#ifndef PROBABILITE_LAW_H
#define PROBABILITE_LAW_H

#include <poincare.h>
#include <escher.h>
#include "../../shared/curve_view_range.h"
#include "../../i18n.h"

namespace Probability {

class Law : public Shared::CurveViewRange {
public:
  Law();
  enum class Type : uint8_t{
    Binomial,
    Uniform,
    Exponential,
    Normal,
    Poisson
  };
  virtual ~Law() = default;
  virtual I18n::Message title() = 0;
  virtual Type type() const = 0;
  virtual bool isContinuous() const = 0;
  float xGridUnit() override;
  virtual int numberOfParameter() = 0;
  virtual float parameterValueAtIndex(int index) = 0;
  virtual I18n::Message parameterNameAtIndex(int index) = 0;
  virtual I18n::Message parameterDefinitionAtIndex(int index) = 0;
  virtual void setParameterAtIndex(float f, int index) = 0;
  virtual float evaluateAtAbscissa(float x) const = 0;
  virtual bool authorizedValueAtIndex(float x, int index) const = 0;
  virtual float cumulativeDistributiveFunctionAtAbscissa(float x) const;
  float rightIntegralFromAbscissa(float x) const;
  float finiteIntegralBetweenAbscissas(float a, float b) const;
  virtual float cumulativeDistributiveInverseForProbability(float * probability);
  virtual float rightIntegralInverseForProbability(float * probability);
protected:
  constexpr static int k_maxNumberOfOperations = 1000000;
  constexpr static float k_maxProbability = 0.9995f;
  constexpr static float k_displayTopMarginRatio = 0.05f;
  constexpr static float k_displayBottomMarginRatio = 0.2f;
  constexpr static float k_displayLeftMarginRatio = 0.05f;
  constexpr static float k_displayRightMarginRatio = 0.05f;
};

}

#endif
