#ifndef PROBABILITY_MODELS_STATISTIC_CHI2_TEST_H
#define PROBABILITY_MODELS_STATISTIC_CHI2_TEST_H

#include <poincare/horizontal_layout.h>

#include <cmath>

#include "interfaces/distributions.h"
#include "table.h"
#include "test.h"

namespace Inference {

class Chi2Test : public Test, public Table {
public:
  Chi2Test();

  SignificanceTestType significanceTestType() const override { return SignificanceTestType::Categorical; }
  DistributionType distributionType() const override { return DistributionType::Chi2; }
  bool initializeCategoricalType(CategoricalType type);

  Poincare::Layout testCriticalValueSymbol(const KDFont * font = KDFont::LargeFont) override {
    return DistributionChi2::TestCriticalValueSymbol(font);
  }

  float canonicalDensityFunction(float x) const override {
    return DistributionChi2::CanonicalDensityFunction(x, m_degreesOfFreedom);
  }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override {
    return DistributionChi2::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom);
  }
  double cumulativeDistributiveInverseForProbability(double proba) const override {
    return DistributionChi2::CumulativeNormalizedInverseDistributionFunction(proba, m_degreesOfFreedom);
  }

  bool authorizedParameterAtIndex(double p, int i) const override;

protected:
  using Test::parameterAtIndex;  // Hidden

  // Instanciate unused abstract classs
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::HorizontalLayout::Builder(), I18n::Message::Default};
  }

  // Chi2 specific
  virtual double expectedValue(int index) const = 0;
  virtual double observedValue(int index) const = 0;
  virtual int numberOfValuePairs() const = 0;
  double computeChi2();

private:
  // Inference
  float computeXMin() const override { return DistributionChi2::XMin(m_degreesOfFreedom); }
  float computeXMax() const override { return DistributionChi2::XMax(m_degreesOfFreedom); }
  float computeYMax() const override {
    return DistributionChi2::YMax(m_degreesOfFreedom);
  }

  // Table
  void tableSetParameterAtIndex(double p, int index) override { return setParameterAtIndex(p, index); }
  double tableParameterAtIndex(int index) const override { return parameterAtIndex(index); }
  bool tableAuthorizedParameterAtIndex(double p, int i) const override { return authorizedParameterAtIndex(p, i); }
  int numberOfTableParameters() const override { return numberOfStatisticParameters(); }

};

}  // namespace Inference

#endif /* PROBABILITY_MODELS_STATISTIC_CHI2_TEST_H */
