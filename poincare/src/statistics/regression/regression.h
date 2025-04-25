#ifndef POINCARE_REGRESSION_REGRESSION_H
#define POINCARE_REGRESSION_REGRESSION_H

#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/old/context.h>
#include <poincare/preferences.h>
#include <poincare/src/statistics/data_table.h>
#include <poincare/src/statistics/dataset_adapter.h>
#include <stdint.h>

#include <array>

namespace Poincare {

namespace Internal {

// TODO: Should this follow the Distributions path and remove all inheritance ?
class Regression {
 public:
  // TODO: the order is used by the regression app menu, make it independent
  enum class Type : uint8_t {
    None = 0,
    LinearAxpb,
    Proportional,
    Quadratic,
    Cubic,
    Quartic,
    Logarithmic,
    ExponentialAebx,
    ExponentialAbx,
    Power,
    Trigonometric,
    Logistic,
    Median,
    LinearApbx,
    /* This is an internal model to allow better fitting on the Logistic model.
     * It is equivalent to Logistic but with transformed coefficients
     * It should not be shown to the user */
    LogisticInternal,  // Does not count in k_numberOfModels
  };
  constexpr static int k_numberOfModels = 14;
  constexpr static int k_maxNumberOfCoefficients = 5;  // Quartic model

  using Coefficients = std::array<double, k_maxNumberOfCoefficients>;

  explicit constexpr Regression(size_t initialParametersIterations = 1)
      : m_initialParametersIterations{initialParametersIterations} {}

  // AngleUnit is needed for Trig regression
  static const Regression* Get(Type type, Preferences::AngleUnit angleUnit);

#if TARGET_POINCARE_JS
  /* Not useful since all regression objects are constexpr but silence embind
   * warning. */
  virtual ~Regression() = default;
#endif

  constexpr static char k_xSymbol = 'x';

  // --- Static properties ---

  // - Coefficients
  static int NumberOfCoefficients(Type type);
  constexpr static bool HasCoefficients(Type type) {
    return type != Type::None;
  }

  // - Category
  constexpr static bool IsLinear(Type type) {
    return type == Type::LinearApbx || type == Type::LinearAxpb;
  }
  constexpr static bool IsAffine(Type type) {
    return type == Type::Median || IsLinear(type);
  }
  constexpr static bool IsPolynomial(Type type) {
    return type == Type::Quadratic || type == Type::Cubic ||
           type == Type::Quartic;
  }
  constexpr static bool IsTransformed(Type type) {
    return type == Type::ExponentialAbx || type == Type::ExponentialAebx ||
           type == Type::Power || type == Type::Logarithmic;
  }

  constexpr static bool CanDefaultToConstant(Type type) {
    assert(type != Type::None);
    // Cannot fit constant
    if (type == Type::Proportional) {
      return false;
    }
    // Has custom fit function that ensures R2 maximality
    if (type == Type::Power || type == Type::ExponentialAebx ||
        type == Type::ExponentialAbx || type == Type::LinearApbx ||
        type == Type::LinearAxpb || type == Type::Logarithmic) {
      return false;
    }
    // Has custom fit function that maximizes other quantities
    if (type == Type::Median) {
      return false;
    }
    assert(type == Type::Quadratic || type == Type::Cubic ||
           type == Type::Quartic || type == Type::Logistic ||
           type == Type::LogisticInternal || type == Type::Trigonometric);
    return true;
  }
  // - Correlation and determination coefficients
  constexpr static bool HasR(Type type) {
    return type == Type::None || IsLinear(type) || FitsLnY(type) ||
           FitsLnX(type);
  }
  constexpr static bool HasRSquared(Type type) {
    return HasCoefficients(type) && HasR(type);
  }
  constexpr static bool HasR2(Type type) {
    return type == Type::Proportional || type == Type::Quadratic ||
           type == Type::Cubic || type == Type::Quartic;
  }

  // - Transformation
  constexpr static bool FitsLnY(Type type) {
    // These models are fitted with a ln(+-Y) change of variable.
    return type == Type::Power || type == Type::ExponentialAbx ||
           type == Type::ExponentialAebx;
  }
  constexpr static bool FitsLnX(Type type) {
    // These models are fitted with a ln(X) change of variable.
    return type == Type::Power || type == Type::Logarithmic;
  }
  constexpr static bool FitsLnB(Type type) {
    // These models are fitted with a ln(B) change of variable.
    return type == Type::ExponentialAbx;
  }
  constexpr static bool FitsXOffset(Type type) {
    // These models are fitted with a X-X.mean() change of variable.
    // TODO? Add every other model where an offset in x can be merged to coefs
    return IsPolynomial(type);
  }
  constexpr static bool FitsYOffset(Type type) {
    // These models are fitted with a Y-Y.mean() change of variable.
    // TODO? Add every other model where y=f(x)+constant
    return IsPolynomial(type);
  }

  // - Formula
  static const char* Formula(Type type);
  static const Poincare::Layout TemplateLayout(Type type);

  // --- Methods ---

  virtual Type type() const = 0;

  bool hasR() const { return HasR(type()); }
  bool hasRSquared() const { return HasRSquared(type()); }
  bool hasR2() const { return HasR2(type()); }

  int numberOfCoefficients() const { return NumberOfCoefficients(type()); }
  const char* formula() const { return Formula(type()); }
  Poincare::Layout templateLayout() const { return TemplateLayout(type()); }

  /* TODO: in the following functions, modelCoefficients is sometimes an input
   * parameter, and sometimes an output parameter. It would be much better for
   * clarity purposes to apply the following:
   * - when modelCoefficients is a read-only input parameter, pass it as a
   * "const Coefficients&". It would be more meaningful than "const double*",
   * as it would highlight the fact that modelCoefficients is an array and not
   * only a pointer to a double.
   * - when modelCoefficients is an output parameter, return it instead of
   * having it in the list of function parameters. The return type would be
   * "Coefficients". */

  Poincare::Layout equationLayout(
      const double* modelCoefficients, const char* ySymbol,
      int significantDigits,
      Poincare::Preferences::PrintFloatMode displayMode) const;
  Poincare::UserExpression expression(const double* modelCoefficients) const;

  double evaluate(const double* modelCoefficients, double x) const;

  virtual double levelSet(const double* modelCoefficients, double xMin,
                          double xMax, double y,
                          Poincare::Context* context) const;
  void fit(const Series* series, double* modelCoefficients,
           Poincare::Context* context) const;

  static Coefficients CoefficientsToMatchMean(const Series* series, Type type);
  double correlationCoefficient(const Series* series) const;
  double determinationCoefficient(const Series* series,
                                  const double* modelCoefficients) const;

  double residualAtIndex(const Series* series, const double* modelCoefficients,
                         int index) const;
  double residualStandardDeviation(const Series* series,
                                   const double* modelCoefficients) const;

 protected:
  virtual Poincare::UserExpression privateExpression(
      const double* modelCoefficients) const = 0;

  /* Evaluate cannot use the expression and approximate it since it would be
   * too time consuming. */
  virtual double privateEvaluate(const Coefficients& modelCoefficients,
                                 double x) const = 0;

  // Fit

  /* For some regressions (e.g. trigonometric), fit can be attempted several
   * times with different sets of initial parameters, then the best model
   * among the different fit attempts is selected. */
  size_t m_initialParametersIterations;

  virtual Coefficients privateFit(const Series* series,
                                  Poincare::Context* context) const;
  virtual bool dataSuitableForFit(const Series* series) const;
  constexpr static int k_maxNumberOfPairs = 100;

  class OffsetSeries : public Series {
   public:
    OffsetSeries(const Series* series, bool xOffset = false,
                 bool yOffset = false)
        : m_series(series),
          m_xOffset(xOffset ? StatisticsDatasetFromTable(series, 0).mean()
                            : 0.),
          m_yOffset(yOffset ? StatisticsDatasetFromTable(series, 1).mean()
                            : 0.) {}
    double getX(int i) const override { return m_series->getX(i) - m_xOffset; }
    double getY(int i) const override { return m_series->getY(i) - m_yOffset; }
    int numberOfPairs() const override { return m_series->numberOfPairs(); }

    const double GetXOffset() const { return m_xOffset; }
    const double GetYOffset() const { return m_yOffset; }

   private:
    const Series* m_series;
    const double m_xOffset;
    const double m_yOffset;
  };

  virtual void offsetCoefficients(Coefficients& modelCoefficients,
                                  const OffsetSeries* series) const {
    assert(series->GetXOffset() == 0);
    assert(series->GetYOffset() == 0);
  };

 private:
  // Model attributes
  virtual double partialDerivate(const Coefficients& modelCoefficients,
                                 int derivateCoefficientIndex, double x) const {
    assert(false);
    return 0.0;
  };

  double privateResidualAtIndex(const Series* series,
                                const Coefficients& modelCoefficients,
                                int index) const;
  double privateResidualsSquareSum(const Series* series,
                                   const Coefficients& modelCoefficients) const;
  double privateResidualStandardDeviation(
      const Series* series, const Coefficients& modelCoefficients) const;

  virtual bool isRegressionBetter(
      double residualsSquareSum1, double residualsSquareSum2,
      const Regression::Coefficients& /* modelCoefficients1 */,
      const Regression::Coefficients& /* modelCoefficients2 */) const {
    return residualsSquareSum1 < residualsSquareSum2;
  }

  // Levenberg-Marquardt
  constexpr static double k_maxIterations = 300;
  constexpr static double k_maxMatrixInversionFixIterations = 10;
  constexpr static double k_initialLambda = 0.001;
  constexpr static double k_lambdaFactor = 10;
  constexpr static double k_chi2ChangeCondition = 0.001;
  constexpr static double k_initialCoefficientValue = 1.0;
  constexpr static int k_consecutiveSmallChi2ChangesLimit = 10;
  void fitLevenbergMarquardt(const Series* series,
                             Coefficients& modelCoefficients,
                             Poincare::Context* context) const;
  double chi2(const Series* series,
              const Coefficients& modelCoefficients) const;
  double alphaPrimeCoefficient(const Series* series,
                               const Coefficients& modelCoefficients, int k,
                               int l, double lambda) const;
  double alphaCoefficient(const Series* series,
                          const Coefficients& modelCoefficients, int k,
                          int l) const;
  double betaCoefficient(const Series* series,
                         const Coefficients& modelCoefficients, int k) const;
  int solveLinearSystem(double* solutions, double* coefficients,
                        double* constants, int solutionDimension,
                        Poincare::Context* context) const;
  Coefficients initCoefficientsForFit(double defaultValue,
                                      bool forceDefaultValue,
                                      size_t attemptNumber,
                                      const Series* s = nullptr) const;
  virtual Coefficients specializedInitCoefficientsForFit(
      double defaultValue, size_t /* attemptNumber */,
      const Series* s = nullptr) const;
  virtual void uniformizeCoefficientsFromFit(
      Coefficients& modelCoefficients) const {}
};

}  // namespace Internal

}  // namespace Poincare

#endif
