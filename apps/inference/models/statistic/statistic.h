#ifndef INFERENCE_MODELS_STATISTIC_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_STATISTIC_H

#include <apps/shared/global_context.h>
#include <apps/shared/inference.h>
#include <poincare/statistics/distribution.h>
#include <poincare/statistics/inference.h>

#include "messages.h"
#include "table.h"

namespace Inference {

namespace PcrInference = Poincare::Inference;

/* A Statistic is something that is computed from a sample and whose
 * distribution is known. From its distribution, we can compute statistical test
 * results and confidence intervals.
 */

/* TODO: The whole class hierarchy below this is very cumbersome.
 * I already partially simplified it but it could be further simplified.
 * The main problem is that we have a diamond between subclasses that inherit
 * from Table and Test/Interval.
 * This whole thing should use composition instead of inheritance.
 * */

class Statistic : public Shared::Inference {
 public:
  Statistic() : m_threshold(-1), m_degreesOfFreedom(NAN) {}

  virtual void init() {}
  virtual void tidy() {}

  using SubApp = PcrInference::Method;
  constexpr static int k_numberOfSubApps = 2;

  /* This poor man's RTTI is required only to avoid reinitializing the model
   * everytime we enter a subapp. */
  constexpr virtual SubApp subApp() const = 0;
  constexpr virtual PcrInference::TestType testType() const = 0;
  constexpr virtual PcrInference::StatisticType statisticType() const = 0;
  constexpr virtual PcrInference::CategoricalType categoricalType() const {
    // Default value
    return PcrInference::CategoricalType::Homogeneity;
  }
  constexpr PcrInference::Type type() const {
    return PcrInference::Type(testType(), statisticType(), categoricalType());
  }

  bool initializeSubApp(SubApp subApp);
  bool initializeTest(PcrInference::TestType testType);
  bool initializeDistribution(PcrInference::StatisticType statisticType);
  bool initializeCategoricalType(PcrInference::CategoricalType type);

  int numberOfSignificanceTestTypes() const {
    return PcrInference::NumberOfTestsForMethod(subApp());
  }
  int numberOfAvailableStatistics() const {
    return PcrInference::NumberOfStatisticsForTest(testType());
  }

  constexpr I18n::Message statisticTitle() const {
    return GetMessage(subApp(), Message::Title);
  }
  constexpr I18n::Message statisticBasicTitle() const {
    return GetMessage(subApp(), Message::BasicTitle);
  }
  constexpr I18n::Message tStatisticMessage() const {
    return GetMessage(subApp(), Message::TStatisticMessage);
  }
  constexpr I18n::Message zStatisticMessage() const {
    return GetMessage(subApp(), Message::ZStatisticMessage);
  }
  constexpr I18n::Message tOrZStatisticMessage() const {
    return GetMessage(subApp(), Message::TOrZStatisticMessage);
  }
  constexpr I18n::Message tDistributionName() const {
    return GetMessage(subApp(), Message::TDistributionName);
  }
  constexpr I18n::Message tPooledDistributionName() const {
    return GetMessage(subApp(), Message::TPooledDistributionName);
  }
  constexpr I18n::Message zDistributionName() const {
    return GetMessage(subApp(), Message::ZDistributionName);
  }
  constexpr I18n::Message thresholdName() const {
    return GetMessage(subApp(), Message::ThresholdName);
  }
  constexpr I18n::Message thresholdDescription() const {
    return GetMessage(subApp(), Message::ThresholdDescription);
  }
  constexpr I18n::Message distributionTitle() const {
    return DistributionTitle(testType());
  }
  I18n::Message title() const override final { return Title(type()); }

  virtual void setGraphTitle(char* buffer, size_t bufferSize) const = 0;
  virtual void setResultTitle(char* buffer, size_t bufferSize,
                              bool resultIsTopPage) const {}

  bool hasHypothesisParameters() const {
    return subApp() == SubApp::SignificanceTest &&
           PcrInference::HasHyphothesis(testType());
  }

  // Input
  int numberOfParameters() override {
    return numberOfTestParameters() + 1 /* threshold */;
  }
  virtual int numberOfTestParameters() const {
    return PcrInference::NumberOfParameters(testType());
  }
  int indexOfThreshold() const { return numberOfTestParameters(); }

  bool authorizedParameterAtIndex(double p, int i) const override;
  bool areParametersValid();
  virtual bool validateInputs(int pageIndex = 0) {
    return areParametersValid();
  }
  double parameterAtIndex(int i) const override;

  virtual double preProcessParameter(double p, int index) const { return p; }
  void setParameterAtIndex(double f, int i) override;
  virtual void initParameters();

  double cumulativeDistributiveFunctionAtAbscissa(
      double x) const override final;
  double cumulativeDistributiveInverseForProbability(
      double probability) const override final;
  double threshold() const {
    assert(0 <= m_threshold && m_threshold <= 1);
    return m_threshold;
  }
  void setThreshold(double s) { m_threshold = s; }

  bool canChooseDataset() const {
    return testType() == PcrInference::TestType::OneMean ||
           testType() == PcrInference::TestType::TwoMeans ||
           testType() == PcrInference::TestType::Slope;
  }
  bool hasTable() const {
    return canChooseDataset() || testType() == PcrInference::TestType::Chi2;
  }
  virtual Table* table() {
    assert(false);
    return nullptr;
  }

  const char* criticalValueSymbol() const {
    return PcrInference::CriticalValueSymbol(statisticType());
  }
  Poincare::Layout criticalValueLayout() const {
    return PcrInference::CriticalValueLayout(subApp(), statisticType());
  }

  // Outputs
  int numberOfResults() const {
    return numberOfInferenceResults() + numberOfExtraResults();
  }
  // Extra results are the first section
  int secondResultSectionStart() const { return numberOfExtraResults(); }
  void resultAtIndex(int index, double* value, Poincare::Layout* message,
                     I18n::Message* subMessage, int* precision);

  bool hasDegreeOfFreedom() const {
    return PcrInference::HasDegreesOfFreedom(type());
  }
  double degreeOfFreedom() const { return m_degreesOfFreedom; }

  // Computation
  virtual void compute() = 0;
  using Inference::computeCurveViewRange;

  // CurveViewRange
  virtual bool isGraphable() const = 0;

 protected:
  Poincare::Distribution::Type distributionType() const {
    return PcrInference::DistributionType(statisticType());
  }
  Poincare::Distribution::ParametersArray<double> distributionParameters()
      const {
    return PcrInference::DistributionParameters(statisticType(),
                                                m_degreesOfFreedom);
  }

  Shared::ParameterRepresentation paramRepresentationAtIndex(
      int i) const override final {
    return Shared::ParameterRepresentation{
        PcrInference::ParameterLayout(type(), i),
        ParameterDescriptionAtIndex(type(), i)};
  }

  // Critical value, degrees of freedom, etc.
  virtual int numberOfInferenceResults() const = 0;
  // Some statistics have extra results (e.g. the ones inputted with datasets)
  virtual int numberOfExtraResults() const { return 0; }
  virtual void inferenceResultAtIndex(int index, double* value,
                                      Poincare::Layout* message,
                                      I18n::Message* subMessage,
                                      int* precision) = 0;
  virtual void extraResultAtIndex(int index, double* value,
                                  Poincare::Layout* message,
                                  I18n::Message* subMessage, int* precision) {
    assert(numberOfExtraResults() == 0);
  }

  float computeYMax() const override final;
  float canonicalDensityFunction(float x) const;

  // TODO: WARNING not working with Chi2
  const PcrInference::ParametersArray constParametersArray() const {
    PcrInference::ParametersArray array;
    const double* paramsArray = const_cast<Statistic*>(this)->parametersArray();
    std::copy(paramsArray, paramsArray + PcrInference::k_maxNumberOfParameters,
              array.data());
    return array;
  }

  /* Threshold is either the confidence level or the significance level */
  double m_threshold;
  double m_degreesOfFreedom;
};

}  // namespace Inference

#endif
