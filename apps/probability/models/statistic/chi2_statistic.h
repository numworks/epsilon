#ifndef PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H

#include <poincare/horizontal_layout.h>

#include <cmath>

#include "cached_statistic.h"

namespace Probability {

class Chi2Statistic : public CachedStatistic {
public:
  Chi2Statistic();
  Poincare::Layout testCriticalValueSymbol() override;
  bool hasDegreeOfFreedom() override { return true; }
  double degreeOfFreedom() override { return m_degreesOfFreedom; }
  int indexOfDegreeOfFreedom() { return indexOfThreshold() + 1; }
  bool isValidParamAtIndex(int i, double p) override;
  void setParamAtIndex(int i, double p) override;

  float canonicalDensityFunction(float x) const override;
  double cumulativeNormalizedDistributionFunction(double x) const override;
  double cumulativeNormalizedInverseDistributionFunction(double proba) const override;

  virtual void setParameterAtPosition(int row, int column, double value) = 0;
  virtual double parameterAtPosition(int row, int column) = 0;
  virtual bool isValidParameterAtPosition(int row, int column, double p) = 0;
  /* Delete parameter at location, return true if the deleted param was the last
   * non-deleted value of its row or column. */
  virtual bool deleteParameterAtPosition(int row, int column) = 0;
  virtual void recomputeData() = 0;
  virtual int maxNumberOfColumns() const = 0;
  virtual int maxNumberOfRows() const = 0;

  void computeInterval() override {}

  float xMin() const override;
  float xMax() const override;
  float yMax() const override;

  constexpr static float k_undefinedValue = NAN;

protected:
  using CachedStatistic::setParamAtIndex;  // Hidden
  using CachedStatistic::paramAtIndex;  // Hidden
  double m_degreesOfFreedom;

  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::HorizontalLayout::Builder(), I18n::Message::Default};
  }

  // Chi2 specific
  virtual double expectedValue(int index) = 0;
  virtual double observedValue(int index) = 0;
  virtual int numberOfValuePairs() = 0;

  double computeChi2();
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_CHI2_STATISTIC_H */
