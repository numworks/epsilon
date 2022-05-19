#ifndef PROBABILITY_MODELS_STATISTIC_SLOPE_T_INTERVAL_H
#define PROBABILITY_MODELS_STATISTIC_SLOPE_T_INTERVAL_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "interval.h"
#include "slope_t_statistic.h"

namespace Inference {

class SlopeTInterval : public Interval, public SlopeTStatistic {
public:
  SlopeTInterval(Shared::GlobalContext * context) : SlopeTStatistic(context) {}
  void tidy() override { DoublePairStore::tidy(); }
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::Slope; }
  DistributionType distributionType() const override { return DistributionType::T; }
  I18n::Message title() const override { return SlopeTStatistic::title(); }

  // Inference
  bool authorizedParameterAtPosition(double p, int row, int column) const override { return Inference::authorizedParameterAtIndex(p, index2DToIndex(row, column)); }
  bool authorizedParameterAtIndex(double p, int i) const override { return Inference::authorizedParameterAtIndex(p, i) && SlopeTStatistic::authorizedParameterAtIndex(p, i); }
  bool validateInputs() override { return SlopeTStatistic::validateInputs(); }

  void compute() override {  }

  // Distribution: t
  const char * estimateSymbol() const override { return "x̅"; }
  Poincare::Layout testCriticalValueSymbol(const KDFont * font = KDFont::LargeFont) override { return DistributionT::TestCriticalValueSymbol(font); }
  float canonicalDensityFunction(float x) const override { return DistributionT::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionT::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double p) const override { return DistributionT::CumulativeNormalizedInverseDistributionFunction(p, m_degreesOfFreedom); }

private:
  // Significance Test: Slope
  int numberOfStatisticParameters() const override { return numberOfTableParameters(); }
  ParameterRepresentation paramRepresentationAtIndex(int i) const override {
    return ParameterRepresentation{Poincare::HorizontalLayout::Builder(), I18n::Message::Default};
  }

  // Inference
  double * parametersArray() override {
    assert(false);
    return nullptr;
  }

  // Distribution: t
  float computeYMax() const override { return DistributionT::YMax(m_degreesOfFreedom); }

};

}  // namespace Inference

#endif /* PROBABILITY_MODELS_STATISTIC_ONE_MEAN_T_TEST_H */

