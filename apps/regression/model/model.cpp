#include "model.h"
#include "../store.h"
#include <poincare/complex.h>
#include <poincare/matrix.h>
#include <poincare/multiplication.h>
#include <math.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

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
  double currentChi2 = chi2(store, series, modelCoefficients);
  double lambda = k_initialLambda;
  int n = numberOfCoefficients(); // n unknown coefficients
  int smallChi2ChangeCounts = 0;
  int iterationCount = 0;
  while (smallChi2ChangeCounts < k_consecutiveSmallChi2ChangesLimit && iterationCount < k_maxIterations) {
    // Compute modelCoefficients step
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
    double modelCoefficientSteps[Model::k_maxNumberOfCoefficients];
    solveLinearSystem(modelCoefficientSteps, coefficientsAPrime, operandsB, n, context);
    // Compare new chi2 with the previous value
    double newModelCoefficients[Model::k_maxNumberOfCoefficients];
    for (int i = 0; i < n; i++) {
      newModelCoefficients[i] = modelCoefficients[i] + modelCoefficientSteps[i];
    }
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
  // TODO Choose which formula to use
#if 0
  // Levenberg method
  double result = k == l ? alphaCoefficient(store, series, modelCoefficients, k, l)*(1+lambda) : alphaCoefficient(store, series, modelCoefficients, l, k);
#else
#if 0
  // Marquardt method
  double result = k == l ? alphaCoefficient(store, series, modelCoefficients, k, l)+lambda : alphaCoefficient(store, series, modelCoefficients, l, k);
#else
  // Mixed method to prevent non invertible matrices
  double result = 0.0;
  if (k == l) {
    result = alphaCoefficient(store, series, modelCoefficients, k, l)*(1.0+lambda);
    if (result == 0) {
      result = lambda;
    }
  } else {
    result = alphaCoefficient(store, series, modelCoefficients, l, k);
  }
#endif
#endif
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


// TODO should return an error if no solution ?
void Model::solveLinearSystem(double * solutions, double * coefficients, double * constants, int solutionDimension, Context * context) {
  int n = solutionDimension;
  int inverseResult = Matrix::ArrayInverse(coefficients, n, n);
  assert(inverseResult >= 0);
  Multiplication::computeOnArrays<double>(coefficients, constants, solutions, n, n, 1);
}

}

