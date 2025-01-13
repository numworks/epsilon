#ifndef INFERENCE_MODELS_STATISTIC_INTERFACES_SIGNIFICANCE_TESTS_H
#define INFERENCE_MODELS_STATISTIC_INTERFACES_SIGNIFICANCE_TESTS_H

#include <apps/shared/inference.h>
#include <omg/round.h>

namespace Inference {

class Statistic;
class Test;
class Interval;
enum class DistributionType;

class SignificanceTest {
 public:
  static double ComputePValue(Test* t);
  static bool ValidThreshold(double p);
  static bool InitializeDistribution(Statistic* statistic,
                                     DistributionType type);
};

class OneMean : public SignificanceTest {
 public:
  enum class Type { T, Z };
  enum ParamsOrder { x, s, n };

  // Initialization
  static bool TestInitializeDistribution(Statistic* statistic,
                                         DistributionType distributionType);
  static bool IntervalInitializeDistribution(Statistic* statistic,
                                             DistributionType distributionType);

  // Description
  static I18n::Message Title(Type t) {
    return DispatchTZ(t, I18n::Message::HypothesisControllerTitleOneMeanT,
                      I18n::Message::HypothesisControllerTitleOneMeanZ);
  }
  static I18n::Message DistributionTitle() {
    return I18n::Message::TypeControllerTitleOne;
  }

  static const char* HypothesisSymbol() { return "μ"; }
  static const char* EstimateSymbol() { return "x̅"; };

  // Parameters
  static int NumberOfAvailableDistributions() { return 2; }
  static void InitTestParameters(Test* test);
  static void InitIntervalParameters(Type t, Interval* interval) {
    DispatchTZ(t, InitTIntervalParameters, InitZIntervalParameters)(interval);
  }
  static bool AuthorizedParameterAtIndex(Type t, int index, double p) {
    return DispatchTZ(t, TAuthorizedParameterAtIndex,
                      ZAuthorizedParameterAtIndex)(index, p);
  }
  static double ProcessParameterForIndex(double p, int index);
  static int NumberOfParameters() { return k_numberOfParams; }
  static Shared::ParameterRepresentation ParameterRepresentationAtIndex(
      Type t, int index) {
    return DispatchTZ(t, TParameterRepresentationAtIndex,
                      ZParameterRepresentationAtIndex)(index);
  }
  static double X(double* params) { return params[ParamsOrder::x]; }
  static double S(double* params) { return params[ParamsOrder::s]; }
  static double N(double* params) { return params[ParamsOrder::n]; }

  // Computation
  static void ComputeTest(Type t, Test* test) {
    DispatchTZ(t, ComputeTTest, ComputeZTest)(test);
  }
  static void ComputeInterval(Type t, Interval* interval) {
    DispatchTZ(t, ComputeTInterval, ComputeZInterval)(interval);
  }

  constexpr static int k_numberOfParams = 3;

 private:
  template <typename T>
  static inline T DispatchTZ(Type t, T tOption, T zOption) {
    return t == Type::T ? tOption : zOption;
  }

  static void InitTIntervalParameters(Interval* interval);
  static void InitZIntervalParameters(Interval* interval);
  static bool TAuthorizedParameterAtIndex(int index, double p);
  static bool ZAuthorizedParameterAtIndex(int index, double p);
  static void ComputeTTest(Test* test);
  static void ComputeZTest(Test* test);
  static void ComputeTInterval(Interval* interval);
  static void ComputeZInterval(Interval* interval);
  static Shared::ParameterRepresentation TParameterRepresentationAtIndex(
      int index);
  static Shared::ParameterRepresentation ZParameterRepresentationAtIndex(
      int index);

  static double ComputeDegreesOfFreedom(double n) { return n - 1.0; }
  static double ComputeTZ(double mean, double meanSample, double s, double n) {
    return (meanSample - mean) / (s / std::sqrt(n));
  }
  static double ComputeStandardError(double s, double n) {
    return OMG::LaxToZero(s / std::sqrt(n));
  }
};

class OneProportion : public SignificanceTest {
 public:
  enum ParamsOrder { x, n };

  // Description
  static I18n::Message Title() {
    return I18n::Message::HypothesisControllerTitleOneProp;
  }

  static const char* HypothesisSymbol() { return "p"; }
  static const char* EstimateSymbol() { return "p̂"; };
  static Poincare::Layout EstimateLayout(Poincare::Layout* layout);
  static I18n::Message EstimateDescription() {
    return I18n::Message::SampleProportion;
  };

  // Parameters
  static void InitTestParameters(Test* test);
  static void InitIntervalParameters(Interval* interval);
  static bool AuthorizedParameterAtIndex(int index, double p);
  static double ProcessParameterForIndex(double p, int index);
  static bool ValidH0(double h0) { return h0 > 0 && h0 < 1; }
  static bool ValidateInputs(double* params) { return X(params) <= N(params); }

  static int NumberOfParameters() { return k_numberOfParams; }
  static Shared::ParameterRepresentation ParameterRepresentationAtIndex(
      int index);
  static double X(double* params) { return params[ParamsOrder::x]; }
  static double N(double* params) { return params[ParamsOrder::n]; }

  // Computation
  static void ComputeTest(Test* test);
  static void ComputeInterval(Interval* interval);

  constexpr static int k_numberOfParams = 2;

 private:
  static double ComputeZ(double p0, double p, double n);
  static double ComputeStandardError(double pEstimate, double n);
};

class TwoMeans : public SignificanceTest {
 public:
  enum class Type { T, TPooled, Z };
  enum ParamsOrder { x1, s1, n1, x2, s2, n2 };

  // Initialization
  static bool TestInitializeDistribution(Statistic* statistic,
                                         DistributionType distributionType);
  static bool IntervalInitializeDistribution(Statistic* statistic,
                                             DistributionType distributionType);

  static I18n::Message Title(Type t) {
    return DispatchTTPooledZ(
        t, I18n::Message::HypothesisControllerTitleTwoMeansT,
        I18n::Message::HypothesisControllerTitleTwoMeansPooledT,
        I18n::Message::HypothesisControllerTitleTwoMeansZ);
  }
  static I18n::Message DistributionTitle() {
    return I18n::Message::TypeControllerTitleTwo;
  }

  // Description
  static int NumberOfAvailableDistributions() { return 3; }
  static const char* HypothesisSymbol() { return "μ1-μ2"; }
  static const char* EstimateSymbol() { return "x̅1-x̅2"; };
  static Poincare::Layout EstimateLayout(Poincare::Layout* layout);
  static I18n::Message EstimateDescription() {
    return I18n::Message::SampleTwoMeans;
  };

  // Parameters
  static void InitTestParameters(Test* test);
  static void InitIntervalParameters(Interval* interval);
  static bool AuthorizedParameterAtIndex(Type t, int index, double p) {
    return DispatchTZ(t, TAuthorizedParameterAtIndex,
                      ZAuthorizedParameterAtIndex)(index, p);
  }
  static double ProcessParameterForIndex(double p, int index);
  static bool ValidateInputs(Type t, double* params) {
    return DispatchTZ(t, TValidateInputs, ZValidateInputs)(params);
  }

  static int NumberOfParameters() { return k_numberOfParams; }
  static Shared::ParameterRepresentation ParameterRepresentationAtIndex(
      Type t, int index) {
    return DispatchTZ(t, TParameterRepresentationAtIndex,
                      ZParameterRepresentationAtIndex)(index);
  }
  static double X1(double* params) { return params[ParamsOrder::x1]; }
  static double N1(double* params) { return params[ParamsOrder::n1]; }
  static double S1(double* params) { return params[ParamsOrder::s1]; }
  static double X2(double* params) { return params[ParamsOrder::x2]; }
  static double N2(double* params) { return params[ParamsOrder::n2]; }
  static double S2(double* params) { return params[ParamsOrder::s2]; }

  constexpr static ParamsOrder X(uint8_t index) {
    assert(index == 0 || index == 1);
    return index == 0 ? ParamsOrder::x1 : ParamsOrder::x2;
  }

  constexpr static ParamsOrder S(uint8_t index) {
    assert(index == 0 || index == 1);
    return index == 0 ? ParamsOrder::s1 : ParamsOrder::s2;
  }

  constexpr static ParamsOrder N(uint8_t index) {
    assert(index == 0 || index == 1);
    return index == 0 ? ParamsOrder::n1 : ParamsOrder::n2;
  }

  // Computation
  static void ComputeTest(Type t, Test* test) {
    return DispatchTTPooledZ(t, ComputeTTest, ComputePooledTest,
                             ComputeZTest)(test);
  }
  static void ComputeInterval(Type t, Interval* interval) {
    return DispatchTTPooledZ(t, ComputeTInterval, ComputePooledInterval,
                             ComputeZInterval)(interval);
  }

  constexpr static int k_numberOfParams = 6;

 private:
  template <typename T>
  static inline T DispatchTZ(Type t, T tOption, T zOption) {
    return t == Type::Z ? zOption : tOption;
  }
  template <typename T>
  static inline T DispatchTTPooledZ(Type t, T tOption, T tPooledOption,
                                    T zOption) {
    return t == Type::TPooled ? tPooledOption : DispatchTZ(t, tOption, zOption);
  }

  static bool ZAuthorizedParameterAtIndex(int index, double p);
  static bool TAuthorizedParameterAtIndex(int index, double p);
  static bool ZValidateInputs(double* params);
  static bool TValidateInputs(double* params);
  static Shared::ParameterRepresentation ZParameterRepresentationAtIndex(
      int index);
  static Shared::ParameterRepresentation TParameterRepresentationAtIndex(
      int index);
  static void ComputeZTest(Test* test);
  static void ComputeTTest(Test* test);
  static void ComputePooledTest(Test* t);
  static void ComputeZInterval(Interval* interval);
  static void ComputeTInterval(Interval* interval);
  static void ComputePooledInterval(Interval* i);
  static double ComputeTZ(double deltaMean, double meanSample1, double n1,
                          double sigma1, double meanSample2, double n2,
                          double sigma2);
  static double ComputeStandardError(double sigma1, double n1, double sigma2,
                                     double n2);
  static double ComputePooledStandardError(double n1, double s1, double n2,
                                           double s2);
  static double ComputeDegreesOfFreedom(double s1, double n1, double s2,
                                        double n2);
};

class TwoProportions : public SignificanceTest {
 public:
  enum ParamsOrder { x1, n1, x2, n2 };

  static I18n::Message Title() {
    return I18n::Message::HypothesisControllerTitleTwoProps;
  }

  // Description
  static const char* HypothesisSymbol() { return "p1-p2"; }
  static const char* EstimateSymbol() { return "p̂1-p̂2"; }
  static Poincare::Layout EstimateLayout(Poincare::Layout* layout);
  static I18n::Message EstimateDescription() {
    return I18n::Message::SampleTwoProportions;
  }
  static I18n::Message Sample1ProportionDescription() {
    return I18n::Message::Sample1Proportion;
  }
  static I18n::Message Sample2ProportionDescription() {
    return I18n::Message::Sample2Proportion;
  }
  static I18n::Message PooledProportionDescription() {
    return I18n::Message::PooledProportion;
  }

  // Parameters
  static void InitTestParameters(Test* test);
  static void InitIntervalParameters(Interval* interval);
  static bool AuthorizedParameterAtIndex(int index, double p);
  static double ProcessParameterForIndex(double p, int index);
  static bool ValidH0(double h0) { return h0 <= 1 && h0 >= -1; }
  static bool ValidateInputs(double* params);

  static int NumberOfParameters() { return k_numberOfParams; }
  static Shared::ParameterRepresentation ParameterRepresentationAtIndex(
      int index);
  static double X1(double* params) { return params[ParamsOrder::x1]; }
  static double N1(double* params) { return params[ParamsOrder::n1]; }
  static double X2(double* params) { return params[ParamsOrder::x2]; }
  static double N2(double* params) { return params[ParamsOrder::n2]; }

  // Computation
  static void ComputeTest(Test* test);
  static void ComputeInterval(Interval* interval);

  constexpr static int k_numberOfParams = 4;

 private:
  static double ComputeEstimate(double x1, double n1, double x2, double n2) {
    return x1 / n1 - x2 / n2;
  }
  static double ComputeZ(double deltaP0, double x1, double n1, double x2,
                         double n2);
  static double ComputeStandardError(double p1Estimate, double n1,
                                     double p2Estimate, double n2);
};

}  // namespace Inference

#endif
