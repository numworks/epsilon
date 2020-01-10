#ifndef PROBABILITE_DISTRIBUTION_H
#define PROBABILITE_DISTRIBUTION_H

#include "../../shared/curve_view_range.h"
#include <apps/i18n.h>
#include <poincare/preferences.h>

namespace Probability {

class Distribution : public Shared::CurveViewRange {
public:
  Distribution() : Shared::CurveViewRange() {}
  enum class Type : uint8_t{
    Binomial,
    Uniform,
    Exponential,
    Normal,
    ChiSquared,
    Student,
    Geometric,
    Poisson,
    Fisher
  };
  virtual ~Distribution() = default;
  virtual I18n::Message title() = 0;
  virtual Type type() const = 0;
  virtual bool isContinuous() const = 0;
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
  static_assert(Poincare::Preferences::LargeNumberOfSignificantDigits == 7, "k_maxProbability is ill-defined compared to LargeNumberOfSignificantDigits");
  constexpr static double k_maxProbability = 0.9999995;
  constexpr static float k_displayTopMarginRatio = 0.05f;
  constexpr static float k_displayLeftMarginRatio = 0.05f;
  constexpr static float k_displayRightMarginRatio = 0.05f;
  double cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(double * probability, double ax, double bx);
private:
  constexpr static float k_displayBottomMarginRatio = 0.2f;
  float yMin() const override;
};

}

#endif
