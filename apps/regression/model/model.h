#ifndef REGRESSION_MODEL_H
#define REGRESSION_MODEL_H

#include <stdint.h>
#include <apps/i18n.h>
#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/matrix.h>

namespace Regression {

class Store;

class Model {
public:
  enum class Type : uint8_t {
    None            = 0,
    LinearAxpb      = 1,
    Proportional    = 2,
    Quadratic       = 3,
    Cubic           = 4,
    Quartic         = 5,
    Logarithmic     = 6,
    ExponentialAebx = 7,
    ExponentialAbx  = 8,
    Power           = 9,
    Trigonometric   = 10,
    Logistic        = 11,
    Median          = 12,
    LinearApbx      = 13,
  };
  constexpr static int k_numberOfModels = 14;
  constexpr static int k_maxNumberOfCoefficients = 5; // Quartic model
  static_assert(k_maxNumberOfCoefficients*k_maxNumberOfCoefficients <= Poincare::Matrix::k_maxNumberOfChildren, "Model needs bigger than allowed matrices");
  virtual Poincare::Layout layout();

  virtual I18n::Message formulaMessage() const = 0;
  virtual I18n::Message name() const = 0;
  virtual int buildEquationTemplate(char * buffer, size_t bufferSize, double * modelCoefficients, int significantDigits, Poincare::Preferences::PrintFloatMode displayMode) const = 0;
  virtual double evaluate(double * modelCoefficients, double x) const = 0;
  virtual double levelSet(double * modelCoefficients, double xMin, double xMax, double y, Poincare::Context * context);
  void fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context);
  virtual int numberOfCoefficients() const = 0;
protected:
  // Fit
  virtual void privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context);
  virtual bool dataSuitableForFit(Store * store, int series) const;
  Poincare::Layout m_layout;
private:
  // Model attributes
  virtual Poincare::Expression expression(double * modelCoefficients) { return Poincare::Expression(); } // expression is overridden only by Models that do not override levelSet
  virtual double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const = 0;

  void storeRegressionFunction(int series, Poincare::Expression expression) const;
  void deleteRegressionFunction(int series) const { /* TODO */}

  // Levenberg-Marquardt
  constexpr static double k_maxIterations = 300;
  constexpr static double k_maxMatrixInversionFixIterations = 10;
  constexpr static double k_initialLambda = 0.001;
  constexpr static double k_lambdaFactor = 10;
  constexpr static double k_chi2ChangeCondition = 0.001;
  constexpr static double k_initialCoefficientValue = 1.0;
  constexpr static int k_consecutiveSmallChi2ChangesLimit = 10;
  void fitLevenbergMarquardt(Store * store, int series, double * modelCoefficients, Poincare::Context * context);
  double chi2(Store * store, int series, double * modelCoefficients) const;
  double alphaPrimeCoefficient(Store * store, int series, double * modelCoefficients, int k, int l, double lambda) const;
  double alphaCoefficient(Store * store, int series, double * modelCoefficients, int k, int l) const;
  double betaCoefficient(Store * store, int series, double * modelCoefficients, int k) const;
  int solveLinearSystem(double * solutions, double * coefficients, double * constants, int solutionDimension, Poincare::Context * context);
  void initCoefficientsForFit(double * modelCoefficients, double defaultValue, bool forceDefaultValue, Store * store = nullptr, int series = -1) const;
  virtual void specializedInitCoefficientsForFit(double * modelCoefficients, double defaultValue, Store * store = nullptr, int series = -1) const;
  virtual void uniformizeCoefficientsFromFit(double * modelCoefficients) const {}
};

}

#endif
