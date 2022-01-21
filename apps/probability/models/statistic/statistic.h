#ifndef PROBABILITY_MODELS_STATISTIC_STATISTIC_H
#define PROBABILITY_MODELS_STATISTIC_STATISTIC_H

#include <probability/models/inference.h>

namespace Probability {

/* A Statistic is something that is computed from a sample and whose distribution is known.
 * From its distribution, we can compute statistical test results and confidence intervals.
 */

enum class SignificanceTestType {
  OneMean,
  TwoMeans,
  OneProportion,
  TwoProportions,
  Categorical
};

enum class DistributionType {
  T,
  TPooled,
  Z,
  Chi2
};

class Statistic : public Inference {
friend class DistributionInterface;
public:
  Statistic() :
    m_threshold(-1),
    m_degreesOfFreedom(NAN) {}

  enum class Type {
    Probability,
    Tests,
    Intervals
  };

  /* Instantiate correct Statistic based on SignificanceTestType, DistributionType and CategoricalType. */
  virtual void initializeSignificanceTest(SignificanceTestType testType) = 0;
  virtual void initializeDistribution(DistributionType distribution) { assert(false); }
  virtual void initParameters() = 0;

  // Input
  int numberOfParameters() override { return numberOfStatisticParameters() + 1 /* threshold */; }
  double parameterAtIndex(int i) const override;
  void setParameterAtIndex(double f, int i) override;
  double threshold() const { return m_threshold; }
  void setThreshold(double s) { m_threshold = s; }

  int indexOfThreshold() const{ return numberOfStatisticParameters(); }
  virtual void initThreshold() = 0;
  virtual Poincare::Layout testCriticalValueSymbol() = 0;

  // Outputs
  bool hasDegreeOfFreedom() { return !std::isnan(m_degreesOfFreedom); }
  double degreeOfFreedom() { return m_degreesOfFreedom; }

  // Computation
  virtual void compute() = 0;

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
