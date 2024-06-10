#ifndef POINCARE_REGRESSION_REGRESSION_H
#define POINCARE_REGRESSION_REGRESSION_H

#include <poincare/expression.h>
#include <poincare/old/context.h>
#include <poincare/old/matrix.h>
#include <stdint.h>

namespace Poincare::Regression {

// Apps will provide series to the regression algorithm with Series
class Series {
 public:
  virtual double getX(int i) const = 0;
  virtual double getY(int i) const = 0;
  virtual int numberOfPairs() const = 0;
};

class Regression {
 public:
  enum class Type : uint8_t {
    None = 0,
    LinearAxpb,
    LinearApbx,
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
  };
  constexpr static int k_numberOfModels = 14;
  constexpr static int k_maxNumberOfCoefficients = 5;  // Quartic model
  static_assert(k_maxNumberOfCoefficients * k_maxNumberOfCoefficients <=
                    Poincare::Matrix::k_maxNumberOfChildren,
                "Model needs bigger than allowed matrices");

  static const Regression* Get(Type type);

  constexpr static char k_xSymbol = 'x';

  virtual int numberOfCoefficients() const = 0;

  virtual Poincare::Layout templateLayout() const;
  Poincare::Layout equationLayout(
      double* modelCoefficients, const char* ySymbol, int significantDigits,
      Poincare::Preferences::PrintFloatMode displayMode) const;
  Poincare::UserExpression expression(double* modelCoefficients) const;

  /* Evaluate cannot use the expression and approximate it since it would be
   * too time consuming. */
  virtual double evaluate(double* modelCoefficients, double x) const = 0;
  virtual double levelSet(double* modelCoefficients, double xMin, double xMax,
                          double y, Poincare::Context* context);
  void fit(const Series* series, double* modelCoefficients,
           Poincare::Context* context);

 protected:
  virtual Poincare::UserExpression privateExpression(
      double* modelCoefficients) const = 0;

  // Fit
  virtual void privateFit(const Series* series, double* modelCoefficients,
                          Poincare::Context* context);
  virtual bool dataSuitableForFit(const Series* series) const;

 private:
  // Model attributes
  virtual double partialDerivate(double* modelCoefficients,
                                 int derivateCoefficientIndex, double x) const {
    assert(false);
    return 0.0;
  };

  // Levenberg-Marquardt
  constexpr static double k_maxIterations = 300;
  constexpr static double k_maxMatrixInversionFixIterations = 10;
  constexpr static double k_initialLambda = 0.001;
  constexpr static double k_lambdaFactor = 10;
  constexpr static double k_chi2ChangeCondition = 0.001;
  constexpr static double k_initialCoefficientValue = 1.0;
  constexpr static int k_consecutiveSmallChi2ChangesLimit = 10;
  void fitLevenbergMarquardt(const Series* series, double* modelCoefficients,
                             Poincare::Context* context);
  double chi2(const Series* series, double* modelCoefficients) const;
  double alphaPrimeCoefficient(const Series* series, double* modelCoefficients,
                               int k, int l, double lambda) const;
  double alphaCoefficient(const Series* series, double* modelCoefficients,
                          int k, int l) const;
  double betaCoefficient(const Series* series, double* modelCoefficients,
                         int k) const;
  int solveLinearSystem(double* solutions, double* coefficients,
                        double* constants, int solutionDimension,
                        Poincare::Context* context);
  void initCoefficientsForFit(double* modelCoefficients, double defaultValue,
                              bool forceDefaultValue,
                              const Series* s = nullptr) const;
  virtual void specializedInitCoefficientsForFit(
      double* modelCoefficients, double defaultValue,
      const Series* s = nullptr) const;
  virtual void uniformizeCoefficientsFromFit(double* modelCoefficients) const {}
};

}  // namespace Poincare::Regression

#endif
