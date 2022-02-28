#ifndef PROBABILITY_MODELS_STATISTIC_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_STATISTIC_H

#include <probability/models/inference.h>
#include "hypothesis_params.h"

namespace Probability {

/* A Statistic is something that is computed from a sample and whose distribution is known.
 * From its distribution, we can compute statistical test results and confidence intervals.
 */

enum class SignificanceTestType {
  // Order matter for cells order
  OneProportion,
  OneMean,
  TwoProportions,
  TwoMeans,
  Categorical,
  NumberOfSignificanceTestTypes
};

enum class DistributionType {
  T,
  TPooled,
  Z,
  Chi2
};

enum class CategoricalType {
  // Order matter for cells order
  GoodnessOfFit,
  Homogeneity
};

class Statistic : public Inference {
friend class DistributionInterface;
public:
  constexpr static int k_numberOfSignificanceTestType = 5;
  static_assert(k_numberOfSignificanceTestType == static_cast<int>(SignificanceTestType::NumberOfSignificanceTestTypes), "k_numberOfSignificanceTestType doesn't agree with SignificanceTestType::NumberOfSignificanceTestTypes");

  Statistic() :
    m_threshold(-1),
    m_degreesOfFreedom(NAN) {}

  virtual int numberOfSignificancesTestTypes() const { return k_numberOfSignificanceTestType; }
  virtual int numberOfAvailableDistributions() const { return 1; }

  virtual I18n::Message statisticTitle() const = 0;
  virtual I18n::Message statisticBasicTitle() const = 0;
  virtual I18n::Message zStatisticMessage() const = 0;
  virtual I18n::Message tOrZStatisticMessage() const = 0;
  virtual I18n::Message distributionTitle() const { return I18n::Message::Default; }
  virtual I18n::Message distributionDescription() const { return I18n::Message::Default; }
  virtual bool hasHypothesisParameters() const { return false; }
  virtual HypothesisParams * hypothesisParams() { assert(false); return nullptr; }
  virtual const char * hypothesisSymbol() { assert(false); return nullptr; }
  virtual I18n::Message tDistributionName() const = 0;
  virtual I18n::Message tPooledDistributionName() const = 0;
  virtual I18n::Message zDistributionName() const = 0;
  virtual void setGraphTitle(char * buffer, size_t bufferSize) const = 0;
  virtual void setResultTitle(char * buffer, size_t bufferSize, bool resultIsTopPage) const {}

  /* Instantiate correct Statistic based on SignificanceTestType, DistributionType and CategoricalType. */
  virtual SignificanceTestType significanceTestType() const = 0;
  virtual bool initializeSignificanceTest(SignificanceTestType testType);
  virtual DistributionType distributionType() const = 0;
  virtual bool initializeDistribution(DistributionType distribution);
  virtual CategoricalType categoricalType() const { assert(false); return CategoricalType::Homogeneity; }
  virtual void initParameters() = 0;

  // Input
  int numberOfParameters() override { return numberOfStatisticParameters() + 1 /* threshold */; }
  double parameterAtIndex(int i) const override;
  void setParameterAtIndex(double f, int i) override;
  double threshold() const { return m_threshold; }
  void setThreshold(double s) { m_threshold = s; }

  int indexOfThreshold() const{ return numberOfStatisticParameters(); }
  virtual void initThreshold() = 0;
  virtual I18n::Message thresholdName() const = 0;
  virtual I18n::Message thresholdDescription() const = 0;
  virtual Poincare::Layout testCriticalValueSymbol(const KDFont * font = KDFont::LargeFont) = 0;

  // Outputs
  virtual int numberOfResults() const = 0;
  virtual void resultAtIndex(int index, double * value, Poincare::Layout * message, I18n::Message * subMessage) = 0;
  bool hasDegreeOfFreedom() const { return !std::isnan(m_degreesOfFreedom); }
  double degreeOfFreedom() const { return m_degreesOfFreedom; }

  // Computation
  virtual void compute() = 0;
  using Inference::computeCurveViewRange;

  // CurveViewRange
  virtual bool isGraphable() const { return true; }

protected:
  virtual float canonicalDensityFunction(float x) const = 0;
  virtual int numberOfStatisticParameters() const = 0;

  /* Threshold is either the confidence level or the significance level */
  double m_threshold;
  double m_degreesOfFreedom;
};

}  // namespace Probability

#endif /* PROBABILITY_MODELS_STATISTIC_STATISTIC_H */
