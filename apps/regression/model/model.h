#ifndef REGRESSION_MODEL_H
#define REGRESSION_MODEL_H

#include <stdint.h>
#include <poincare/context.h>
#include <poincare/expression.h>

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
  static constexpr int k_maxNumberOfCoefficients = 5;
  virtual ~Model() = default;
  virtual double evaluate(double * modelCoefficients, double x) const = 0;
  virtual double levelSet(double * modelCoefficients, double y) const = 0;
  virtual void fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context);
private:
  // Model attributes
  virtual double partialDerivate(double * modelCoefficients, int derivateCoefficientIndex, double x) const = 0;
  virtual double numberOfCoefficients() const = 0;

  // Levenberg-Marquardt
  static constexpr double k_maxIterations = 1000;
  static constexpr double k_initialLambda = 0.001;
  static constexpr double k_lambdaFactor = 10;
  static constexpr double k_chi2ChangeCondition = 0.0005;
  static constexpr int k_consecutiveSmallChi2ChangesLimit = 10;
  void fitLevenbergMarquardt(Store * store, int series, double * modelCoefficients, Poincare::Context * context);
  double chi2(Store * store, int series, double * modelCoefficients) const;
  double alphaPrimeCoefficient(Store * store, int series, double * modelCoefficients, int k, int l, double lambda) const;
  double alphaCoefficient(Store * store, int series, double * modelCoefficients, int k, int l) const;
  double betaCoefficient(Store * store, int series, double * modelCoefficients, int k) const;
  void solveLinearSystem(double * solutions, Poincare::Expression * coefficients[Model::k_maxNumberOfCoefficients][Model::k_maxNumberOfCoefficients], Poincare::Expression * * constants, int solutionDimension, Poincare::Context * context);
};

}

#endif
