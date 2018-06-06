#ifndef REGRESSION_LEVENBERG_MARQUARDT_H
#define REGRESSION_LEVENBERG_MARQUARDT_H

#include <poincare.h>
#include "model/cubic_model.h"
#include "model/exponential_model.h"
#include "model/linear_model.h"
#include "model/logarithmic_model.h"
#include "model/logistic_model.h"
#include "model/model.h"
#include "model/power_model.h"
#include "model/quadratic_model.h"
#include "model/quartic_model.h"
#include "model/trigonometric_model.h"
#include "store.h"

namespace Regression {

class LevenbergMarquartd {
public:
  LevenbergMarquartd(Store * store) :
    m_store(store),
    m_series(0),
    m_model()
  {
  }
  void fit(double * modelCoefficients, Poincare::Context * context);
private:
  static constexpr double k_maxIterations = 1000;
  static constexpr double k_initialLambda = 0.001;
  static constexpr double k_lambdaFactor = 10;
  static constexpr double k_chi2ChangeCondition = 0.0005;
  static constexpr int k_consecutiveSmallChi2ChangesLimit = 10;

  double chi2(double * modelCoefficients) const;
  double alphaPrimeCoefficient(double * modelCoefficients, int k, int l, double lambda) const;
  double alphaCoefficient(double * modelCoefficients, int k, int l) const;
  double betaCoefficient(double * modelCoefficients, int k) const;
  void solveLinearSystem(double * solutions, Poincare::Expression * coefficients[Model::k_maxNumberOfCoefficients][Model::k_maxNumberOfCoefficients], Poincare::Expression * * constants, int solutionDimension, Poincare::Context * context);

  Store * m_store;
  int m_series; //TODO put as argument
  LogisticModel m_model; //TODO put as argument
};

}


#endif
