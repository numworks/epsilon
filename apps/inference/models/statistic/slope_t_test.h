#ifndef PROBABILITY_MODELS_STATISTIC_SLOPE_T_TEST_H
#define PROBABILITY_MODELS_STATISTIC_SLOPE_T_TEST_H

#include "interfaces/distributions.h"
#include "interfaces/significance_tests.h"
#include "table.h"
#include "test.h"

namespace Inference {

class SlopeTTest : public Test, public Table {
public:
  SignificanceTestType significanceTestType() const override { return SignificanceTestType::Slope; }
  DistributionType distributionType() const override { return DistributionType::T; }
  I18n::Message title() const override { return Slope::Title(); }
  I18n::Message graphTitleFormat() const override { return DistributionT::GraphTitleFormat(); }

  // Inference
  void setParameterAtPosition(double value, int row, int column) override { return Slope::SetParameterAtPosition(value, row, column); }
  double parameterAtPosition(int row, int column) const override { return Slope::ParameterAtPosition(row, column); }
  bool authorizedParameterAtPosition(double p, int row, int column) const override { return authorizedParameterAtIndex(p, index2DToIndex(row, column)); }
  /* Delete parameter at location, return true if the deleted param was the last
   * non-deleted value of its row or column. */
  void recomputeData() override { Slope::RecomputeData(); }
  int maxNumberOfColumns() const override { return k_maxNumberOfColumns; }
  int maxNumberOfRows() const override { return Slope::NumberOfRows(); }

  // Significance Test: Slope
  const char * hypothesisSymbol() override { return Slope::HypothesisSymbol(); }
  bool authorizedParameterAtIndex(double p, int i) const override { return Inference::authorizedParameterAtIndex(p, i) && Slope::AuthorizedParameterAtIndex(p, i); }

  void compute() override { Slope::ComputeTest(this); }

  // Distribution: t
  Poincare::Layout testCriticalValueSymbol(const KDFont * font = KDFont::LargeFont) override { return DistributionT::TestCriticalValueSymbol(font); }
  float canonicalDensityFunction(float x) const override { return DistributionT::CanonicalDensityFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveFunctionAtAbscissa(double x) const override { return DistributionT::CumulativeNormalizedDistributionFunction(x, m_degreesOfFreedom); }
  double cumulativeDistributiveInverseForProbability(double p) const override { return DistributionT::CumulativeNormalizedInverseDistributionFunction(p, m_degreesOfFreedom); }

  constexpr static int k_maxNumberOfColumns = 2;
  // DOUBLE PAIR STORE, k_maxNumberOfPairs
private:
  // Table
  Index2D initialDimensions() const override { return Index2D{.row = 1, .col = 2}; }
  // Significance Test: Slope
  int numberOfStatisticParameters() const override { return Slope::NumberOfParameters(); }
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
