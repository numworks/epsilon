#ifndef REGRESSION_MODEL_H
#define REGRESSION_MODEL_H

#include <stdint.h>
#include <apps/i18n.h>
#include <poincare/context.h>
#include <poincare/expression.h>
#include <poincare/layout.h>

namespace Regression {

class Store;

class Model {
public:
  enum class Type : uint8_t {
    Linear      = 0,
    Quadratic   = 1,
    Cubic       = 2,
    Quartic     = 3,
    Logarithmic = 4,
    Exponential = 5,
    Power       = 6,
    Trigonometric = 7,
    Logistic    = 8
  };
  static constexpr int k_numberOfModels = 9;
  static constexpr int k_maxNumberOfCoefficients = 5; // This has to verify: k_maxNumberOfCoefficients < Matrix::k_maxNumberOfCoefficients
  virtual ~Model() = default;
  virtual Poincare::Layout layout() = 0;
  // Reinitialize m_layout to empty the pool
  void tidy();
  Poincare::Expression simplifiedExpression(double * modelCoefficients, Poincare::Context * context);
  virtual I18n::Message formulaMessage() const = 0;
  virtual double evaluate(double * modelCoefficients, double x) const = 0;
  virtual double levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context);
  virtual void fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context);
  virtual int numberOfCoefficients() const = 0;
  virtual int bannerLinesCount() const { return 2; }
protected:
  // Fit
  virtual bool dataSuitableForFit(Store * store, int series) const;
  constexpr static const KDFont * k_layoutFont = KDFont::SmallFont;
  Poincare::Layout m_layout;
private:
  // Model attributes
  virtual Poincare::Expression expression(double * modelCoefficients) { return Poincare::Expression(); } // expression is overrided only by Models that do not override levelSet
  virtual double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const = 0;

  // Levenberg-Marquardt
  static constexpr double k_maxIterations = 300;
  static constexpr double k_maxMatrixInversionFixIterations = 10;
  static constexpr double k_initialLambda = 0.001;
  static constexpr double k_lambdaFactor = 10;
  static constexpr double k_chi2ChangeCondition = 0.001;
  static constexpr double k_initialCoefficientValue = 1.0;
  static constexpr int k_consecutiveSmallChi2ChangesLimit = 10;
  void fitLevenbergMarquardt(Store * store, int series, double * modelCoefficients, Poincare::Context * context);
  double chi2(Store * store, int series, double * modelCoefficients) const;
  double alphaPrimeCoefficient(Store * store, int series, double * modelCoefficients, int k, int l, double lambda) const;
  double alphaCoefficient(Store * store, int series, double * modelCoefficients, int k, int l) const;
  double betaCoefficient(Store * store, int series, double * modelCoefficients, int k) const;
  int solveLinearSystem(double * solutions, double * coefficients, double * constants, int solutionDimension, Poincare::Context * context);
};

}

#endif
