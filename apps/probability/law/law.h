#ifndef PROBABILITE_LAW_H
#define PROBABILITE_LAW_H

#include <poincare.h>
#include <escher.h>
#include "../../constant.h"
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
  virtual double cumulativeDistributiveFunctionAtAbscissa(double x) const;
  double rightIntegralFromAbscissa(double x) const;
  double finiteIntegralBetweenAbscissas(double a, double b) const;
  virtual double cumulativeDistributiveInverseForProbability(double * probability);
  virtual double rightIntegralInverseForProbability(double * probability);
  virtual double evaluateAtDiscreteAbscissa(int k) const;
  constexpr static int k_maxNumberOfOperations = 1000000;
protected:
  static_assert(Constant::LargeNumberOfSignificantDigits == 7, "k_maxProbability is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_maxProbability = 0.9999995;
  constexpr static float k_displayTopMarginRatio = 0.05f;
  constexpr static float k_displayBottomMarginRatio = 0.2f;
  constexpr static float k_displayLeftMarginRatio = 0.05f;
  constexpr static float k_displayRightMarginRatio = 0.05f;
};

}

#endif
