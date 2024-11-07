#ifndef POINCARE_REGRESSION_REGRESSION_H
#define POINCARE_REGRESSION_REGRESSION_H

#include <poincare/api.h>
#include <poincare/old/context.h>
#include <stdint.h>

#include <array>

#include "series.h"

namespace Poincare::Regression {

class Regression {
 public:
  // TODO: the order is used by the regression app menu, make it independant
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
  };
  constexpr static int k_numberOfModels = 14;
  constexpr static int k_maxNumberOfCoefficients = 5;  // Quartic model

  using CoefficientsType = std::array<double, k_maxNumberOfCoefficients>;

  explicit constexpr Regression(size_t initialParametersIterations = 1)
      : m_initialParametersIterations{initialParametersIterations} {}

  static const Regression* Get(Type type);

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
  constexpr static bool IsTransformed(Type type) {
    return type == Type::ExponentialAbx || type == Type::ExponentialAebx ||
           type == Type::Power || type == Type::Logarithmic;
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
   * "const std::array<double, k_maxNumberOfCoefficients>&". It would be more
   * meaningful than "const double*", as it would highlight the fact that
   * modelCoefficients is an array and not only a pointer to a double.
   * - when modelCoefficients is an output parameter, return it instead of
   * having it in the list of function parameters. The return type would be
   * "std::array<double, k_maxNumberOfCoefficients>"
   */

  Poincare::Layout equationLayout(
      const double* modelCoefficients, const char* ySymbol,
      int significantDigits,
      Poincare::Preferences::PrintFloatMode displayMode) const;
  Poincare::API::UserExpression expression(
      const double* modelCoefficients) const;

  double evaluate(const double* modelCoefficients, double x) const {
    CoefficientsType coefficients;
    std::copy(modelCoefficients, modelCoefficients + numberOfCoefficients(),
              coefficients.begin());
    return privateEvaluate(coefficients, x);
  }

  virtual double levelSet(const double* modelCoefficients, double xMin,
                          double xMax, double y,
                          Poincare::Context* context) const;
  void fit(const Series* series, double* modelCoefficients,
           Poincare::Context* context) const;

  double correlationCoefficient(const Series* series) const;
  double determinationCoefficient(const Series* series,
                                  const double* modelCoefficients) const;

  double residualAtIndex(const Series* series, const double* modelCoefficients,
                         int index) const;
  double residualStandardDeviation(const Series* series,
                                   const double* modelCoefficients) const;

 protected:
  virtual Poincare::API::UserExpression privateExpression(
      const double* modelCoefficients) const = 0;

  /* Evaluate cannot use the expression and approximate it since it would be
   * too time consuming. */
  virtual double privateEvaluate(const CoefficientsType& modelCoefficients,
                                 double x) const = 0;

  // Fit
  /* For some regressions (e.g. trigonometric), fit can be attempted several
   * times with different sets initial parameters, then the best model amoung
   * the different fit attempts is selected. */
  size_t m_initialParametersIterations;
  virtual CoefficientsType privateFit(const Series* series,
                                      Poincare::Context* context) const;
  virtual bool dataSuitableForFit(const Series* series) const;
  constexpr static int k_maxNumberOfPairs = 100;

 private:
  // Model attributes
  virtual double partialDerivate(const CoefficientsType& modelCoefficients,
                                 int derivateCoefficientIndex, double x) const {
    assert(false);
    return 0.0;
  };

  double privateResidualAtIndex(const Series* series,
                                const CoefficientsType& modelCoefficients,
                                int index) const;
  double privateResidualStandardDeviation(
      const Series* series, const CoefficientsType& modelCoefficients) const;

  // Levenberg-Marquardt
  constexpr static double k_maxIterations = 300;
  constexpr static double k_maxMatrixInversionFixIterations = 10;
  constexpr static double k_initialLambda = 0.001;
  constexpr static double k_lambdaFactor = 10;
  constexpr static double k_chi2ChangeCondition = 0.001;
  constexpr static double k_initialCoefficientValue = 1.0;
  constexpr static int k_consecutiveSmallChi2ChangesLimit = 10;
  void fitLevenbergMarquardt(const Series* series,
                             CoefficientsType& modelCoefficients,
                             Poincare::Context* context) const;
  double chi2(const Series* series,
              const CoefficientsType& modelCoefficients) const;
  double alphaPrimeCoefficient(const Series* series,
                               const CoefficientsType& modelCoefficients, int k,
                               int l, double lambda) const;
  double alphaCoefficient(const Series* series,
                          const CoefficientsType& modelCoefficients, int k,
                          int l) const;
  double betaCoefficient(const Series* series,
                         const CoefficientsType& modelCoefficients,
                         int k) const;
  int solveLinearSystem(double* solutions, double* coefficients,
                        double* constants, int solutionDimension,
                        Poincare::Context* context) const;
  CoefficientsType initCoefficientsForFit(double defaultValue,
                                          bool forceDefaultValue,
                                          size_t attemptNumber,
                                          const Series* s = nullptr) const;
  virtual CoefficientsType specializedInitCoefficientsForFit(
      double defaultValue, size_t /* attemptNumber */,
      const Series* s = nullptr) const;
  virtual void uniformizeCoefficientsFromFit(
      CoefficientsType& modelCoefficients) const {}
};

}  // namespace Poincare::Regression

#endif
