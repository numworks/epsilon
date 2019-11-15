#include "model.h"
#include "../store.h"
#include "../../shared/poincare_helpers.h"
#include <poincare/decimal.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <math.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

void Model::tidy() {
  m_layout = Layout();
}

Poincare::Expression Model::simplifiedExpression(double * modelCoefficients, Poincare::Context * context) {
  Expression e = expression(modelCoefficients);
  if (!e.isUninitialized()) {
    PoincareHelpers::Simplify(&e, context, ExpressionNode::ReductionTarget::SystemForApproximation);
  }
  return e;
}

double Model::levelSet(double * modelCoefficients, double xMin, double step, double xMax, double y, Poincare::Context * context) {
  Expression yExpression = Number::DecimalNumber(y);
  PoincareHelpers::Simplify(&yExpression, context, ExpressionNode::ReductionTarget::SystemForApproximation);
  Expression modelExpression = simplifiedExpression(modelCoefficients, context);
  double result = PoincareHelpers::NextIntersection(modelExpression, "x", xMin, step, xMax, context, yExpression).x1();
  return result;
}

void Model::fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) {
  if (dataSuitableForFit(store, series)) {
    for (int i = 0; i < numberOfCoefficients(); i++) {
      modelCoefficients[i] = k_initialCoefficientValue;
    }
    fitLevenbergMarquardt(store, series, modelCoefficients, context);
  } else {
    for (int i = 0; i < numberOfCoefficients(); i++) {
      modelCoefficients[i] = NAN;
    }
  }
}

bool Model::dataSuitableForFit(Store * store, int series) const {
  if (!store->seriesNumberOfAbscissaeGreaterOrEqualTo(series, numberOfCoefficients())) {
    return false;
  }
  return !store->seriesIsEmpty(series);
}

void Model::fitLevenbergMarquardt(Store * store, int series, double * modelCoefficients, Context * context) {
  /* We want to find the best coefficients of the regression to minimize the sum
   * of the squares of the difference between a data point and the corresponding
   * point of the fitting regression (chi2 function).
   * We use the Levenberg-Marquardt algorithm to minimize this chi2 merit
   * function.
   * The equation to solve is A'*da = B, with A' a damped version of the chi2
   * Hessian matrix, da the coefficients increments and B colinear to the
   * gradient of chi2.*/
  double currentChi2 = chi2(store, series, modelCoefficients);
  double lambda = k_initialLambda;
  int n = numberOfCoefficients(); // n unknown coefficients
  int smallChi2ChangeCounts = 0;
  int iterationCount = 0;
  while (smallChi2ChangeCounts < k_consecutiveSmallChi2ChangesLimit && iterationCount < k_maxIterations) {
    // Create the alpha prime matrix (it is symmetric)
    double coefficientsAPrime[Model::k_maxNumberOfCoefficients * Model::k_maxNumberOfCoefficients];
    for (int i = 0; i < n; i++) {
      for (int j = i; j < n; j++) {
        double alphaPrime = alphaPrimeCoefficient(store, series, modelCoefficients, i, j, lambda);
        coefficientsAPrime[i*n+j] = alphaPrime;
        if (i != j) {
          coefficientsAPrime[j*n+i] = alphaPrime;
        }
      }
    }
    // Create the beta matrix
    double operandsB[Model::k_maxNumberOfCoefficients];
    for (int j = 0; j < n; j++) {
      operandsB[j] = betaCoefficient(store, series, modelCoefficients, j);
    }

    // Compute the equation solution (= vector of coefficients increments)
    double modelCoefficientSteps[Model::k_maxNumberOfCoefficients];
    if (solveLinearSystem(modelCoefficientSteps, coefficientsAPrime, operandsB, n, context) < 0) {
      break;
    }

    // Compute the new coefficients
    double newModelCoefficients[Model::k_maxNumberOfCoefficients];
    for (int i = 0; i < n; i++) {
      newModelCoefficients[i] = modelCoefficients[i] + modelCoefficientSteps[i];
    }

    // Compare new chi2 with the previous value
    double newChi2 = chi2(store, series, newModelCoefficients);
    smallChi2ChangeCounts = (fabs(currentChi2 - newChi2) > k_chi2ChangeCondition) ? 0 : smallChi2ChangeCounts + 1;
    if (newChi2 >= currentChi2) {
      lambda*= k_lambdaFactor;
    } else {
      lambda/= k_lambdaFactor;
      for (int i = 0; i < n; i++) {
        modelCoefficients[i] = newModelCoefficients[i];
      }
      currentChi2 = newChi2;
    }
    iterationCount++;
  }
}

double Model::chi2(Store * store, int series, double * modelCoefficients) const {
  double result = 0.0;
  for (int i = 0; i < store->numberOfPairsOfSeries(series); i++) {
    double xi = store->get(series, 0, i);
    double yi = store->get(series, 1, i);
    double difference = yi - evaluate(modelCoefficients, xi);
    result += difference * difference;
  }
  return result;
}

// a'(k,k) = a(k,k) * (1 + lambda)
// a'(k,l) = a(l,k) when (k != l)
double Model::alphaPrimeCoefficient(Store * store, int series, double * modelCoefficients, int k, int l, double lambda) const {
  assert(k >= 0 && k < numberOfCoefficients());
  assert(l >= 0 && l < numberOfCoefficients());
  double result = 0.0;
  if (k == l) {
    /* The Levengerg method uses a'(k,k) = a(k,k) + lambda.
     * The Marquardt method uses a'(k,k) = a(k,k) * (1 + lambda).
     * We use a mixed method to try to make the matrix invertible:
     * a'(k,k) = a(k,k) * (1 + lambda), but if a'(k,k) is too small,
     * a'(k,k) = 2*epsilon so that the inversion method does not detect a'(k,k)
     * as a zero. */
    result = alphaCoefficient(store, series, modelCoefficients, k, l)*(1.0+lambda);
    if (std::fabs(result) < Expression::Epsilon<double>()) {
      result = 2*Expression::Epsilon<double>();
    }
  } else {
    result = alphaCoefficient(store, series, modelCoefficients, l, k);
  }
  return result;
}

// a(k,l) = sum(0, N-1, derivate(y(xi|a), ak) * derivate(y(xi|a), a))
double Model::alphaCoefficient(Store * store, int series, double * modelCoefficients, int k, int l) const {
  assert(k >= 0 && k < numberOfCoefficients());
  assert(l >= 0 && l < numberOfCoefficients());
  double result = 0.0;
  int m = store->numberOfPairsOfSeries(series);
  for (int i = 0; i < m; i++) {
    double xi = store->get(series, 0, i);
    result += partialDerivate(modelCoefficients, k, xi) * partialDerivate(modelCoefficients, l, xi);
  }
  return result;
}

// b(k) = sum(0, N-1, (yi - y(xi|a)) * derivate(y(xi|a), ak))
double Model::betaCoefficient(Store * store, int series, double * modelCoefficients, int k) const {
  assert(k >= 0 && k < numberOfCoefficients());
  double result = 0.0;
  int m = store->numberOfPairsOfSeries(series); // m equations
  for (int i = 0; i < m; i++) {
    double xi = store->get(series, 0, i);
    double yi = store->get(series, 1, i);
    result += (yi - evaluate(modelCoefficients, xi)) * partialDerivate(modelCoefficients, k, xi);
  }
  return result;
}

int Model::solveLinearSystem(double * solutions, double * coefficients, double * constants, int solutionDimension, Context * context) {
  int n = solutionDimension;
  assert(n <= k_maxNumberOfCoefficients);
  double coefficientsSave[k_maxNumberOfCoefficients * k_maxNumberOfCoefficients];
  for (int i = 0; i < n * n; i++) {
    coefficientsSave[i] = coefficients[i];
  }
  assert(k_maxNumberOfCoefficients < Matrix::k_maxNumberOfCoefficients);
  int inverseResult = Matrix::ArrayInverse(coefficients, n, n);
  int numberOfMatrixModifications = 0;
  while (inverseResult < 0 && numberOfMatrixModifications < k_maxMatrixInversionFixIterations) {
    /* If the matrix is not invertible, we modify it to try to make
     * it invertible by multiplying the diagonal coefficients by 1+i/n. This
     * will change the iterative path of the algorithm towards the chi2 minimum,
     * but not the final solution itself, as the stopping condition is that chi2
     * is at its minimum, so when B is null. */
    for (int i = 0; i < n; i ++) {
      coefficientsSave[i*n+i] = (1 + ((double)i)/((double)n)) * coefficientsSave[i*n+i];
    }
    inverseResult = Matrix::ArrayInverse(coefficientsSave, n, n);
    numberOfMatrixModifications++;
  }
  if (inverseResult < 0) {
    return - 1;
  }
  if (numberOfMatrixModifications > 0) {
    for (int i = 0; i < n*n; i++) {
      coefficients[i] = coefficientsSave[i];
    }
  }
  Multiplication::computeOnArrays<double>(coefficients, constants, solutions, n, n, 1);
  return 0;
}

}

