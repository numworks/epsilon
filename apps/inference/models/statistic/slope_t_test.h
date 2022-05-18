#ifndef PROBABILITY_MODELS_STATISTIC_SLOPE_T_TEST_H
#define PROBABILITY_MODELS_STATISTIC_SLOPE_T_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "test.h"
#include "slope_t_statistic.h"

namespace Inference {

class SlopeTTest : public Test, public SlopeTStatistic {
public:
  SlopeTTest(Shared::GlobalContext * context) : SlopeTStatistic(context) {}
  void tidy() override { DoublePairStore::tidy(); }
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::Slope; }
  DistributionType distributionType() const override { return DistributionType::T; }
  I18n::Message title() const override { return Slope::Title(); }
  I18n::Message graphTitleFormat() const override { return DistributionT::GraphTitleFormat(); }

  // Inference
  bool authorizedParameterAtPosition(double p, int row, int column) const override { return Inference::authorizedParameterAtIndex(p, index2DToIndex(row, column)); }
  bool authorizedParameterAtIndex(double p, int i) const override { return Inference::authorizedParameterAtIndex(p, i) && SlopeTStatistic::authorizedParameterAtIndex(p, i); }
  bool validateInputs() override { return seriesIsValid(0); }

  // Significance Test: Slope
  const char * hypothesisSymbol() override { return Slope::HypothesisSymbol(); }

  void compute() override { Slope::ComputeTest(this); }

  // Distribution: t
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
