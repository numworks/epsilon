#include "levenberg_marquardt.h"
#include <poincare/complex.h>
#include <math.h>

using namespace Poincare;
using namespace Shared;

namespace Regression {

void LevenbergMarquartd::fit(double * modelCoefficients, Context * context) {
  double currentChi2 = chi2(modelCoefficients);
  double lambda = k_initialLambda;
  int n = m_model.numberOfCoefficients(); // n unknown coefficients
  int smallChi2ChangeCounts = 0;
  int iterationCount = 0;
  while (smallChi2ChangeCounts < k_consecutiveSmallChi2ChangesLimit && iterationCount < k_maxIterations) {
    // Compute modelCoefficients step
    // Create the alpha prime matrix (it is symmetric)
    Expression * coefficientsAPrime[RegressionModel::k_maxNumberOfCoefficients][RegressionModel::k_maxNumberOfCoefficients]; // TODO find a way not to use so much space, we only need Expression * coefficientsAPrime[n][n]
    for (int i = 0; i < n; i++) {
      for (int j = i; j < n; j++) {
        Complex<double> alphaPrime = Complex<double>::Float(alphaPrimeCoefficient(modelCoefficients, i, j, lambda));
        coefficientsAPrime[i][j] = new Complex<double>(alphaPrime);
        if (i != j) {
          coefficientsAPrime[j][i] = new Complex<double>(alphaPrime);
        }
      }
    }
    // Create the beta matrix
    Expression ** operandsB = new Expression * [n];
    for (int j = 0; j < n; j++) {
      operandsB[j] = new Complex<double>(Complex<double>::Float(betaCoefficient(modelCoefficients, j)));
    }
    double modelCoefficientSteps[n];
    solveLinearSystem(modelCoefficientSteps, coefficientsAPrime, operandsB, n, context);

    // Compare new chi2 with the previous value
    double newModelCoefficients[n];
    for (int i = 0; i < n; i++) {
      newModelCoefficients[i] = modelCoefficients[i] + modelCoefficientSteps[i];
    }
    double newChi2 = chi2(newModelCoefficients);
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

double LevenbergMarquartd::chi2(double * modelCoefficients) const {
  double result = 0.0;
  for (int i = 0; i < m_store->numberOfPairsOfSeries(m_series); i++) {
    double xi = m_store->get(m_series, 0, i);
    double yi = m_store->get(m_series, 1, i);
    double difference = yi - m_model.evaluate(modelCoefficients, xi);
    result += difference * difference;
  }
  return result;
}

// a'(k,k) = a(k,k) * (1 + lambda)
// a'(k,l) = a(l,k) when (k != l)
double LevenbergMarquartd::alphaPrimeCoefficient(double * modelCoefficients, int k, int l, double lambda) const {
  assert(k >= 0 && k < m_model.numberOfCoefficients());
  assert(l >= 0 && l < m_model.numberOfCoefficients());
  double result = k == l ? alphaCoefficient(modelCoefficients, k, l)*(1+lambda) : alphaCoefficient(modelCoefficients, l, k);
  return result;
}

// a(k,l) = sum(0, N-1, derivate(y(xi|a), ak) * derivate(y(xi|a), a))
double LevenbergMarquartd::alphaCoefficient(double * modelCoefficients, int k, int l) const {
  assert(k >= 0 && k < m_model.numberOfCoefficients());
  assert(l >= 0 && l < m_model.numberOfCoefficients());
  double result = 0.0;
  int m = m_store->numberOfPairsOfSeries(m_series);
  for (int i = 0; i < m; i++) {
    double xi = m_store->get(m_series, 0, i);
    result += m_model.partialDerivate(modelCoefficients, k, xi) * m_model.partialDerivate(modelCoefficients, l, xi);
  }
  return result;
}

// b(k) = sum(0, N-1, (yi - y(xi|a)) * derivate(y(xi|a), ak))
double LevenbergMarquartd::betaCoefficient(double * modelCoefficients, int k) const {
  assert(k >= 0 && k < m_model.numberOfCoefficients());
  double result = 0.0;
  int m = m_store->numberOfPairsOfSeries(m_series); // m equations
  for (int i = 0; i < m; i++) {
    double xi = m_store->get(m_series, 0, i);
    double yi = m_store->get(m_series, 1, i);
    result += (yi - m_model.evaluate(modelCoefficients, xi)) * m_model.partialDerivate(modelCoefficients, k, xi);
  }
  return result;
}


// TODO should return an error if no solution ?
void LevenbergMarquartd::solveLinearSystem(double * solutions, Expression * coefficients[RegressionModel::k_maxNumberOfCoefficients][RegressionModel::k_maxNumberOfCoefficients], Expression * * constants, int solutionDimension, Context * context) {
  int n = solutionDimension;
  const Expression ** operandsA = new const Expression * [n*n];
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      operandsA[i*n+j] = coefficients[i][j];
    }
  }
  Matrix * AMatrix = new Matrix(operandsA, n, n, false);
  delete[] operandsA;
  Matrix * BMatrix = new Matrix(constants, n, 1, false);
  Expression::AngleUnit angleUnit = Preferences::sharedPreferences()->angleUnit();
  Matrix * matrixInverse = AMatrix->createApproximateInverse<double>();
  assert(matrixInverse != nullptr);
  Matrix * result = Multiplication::computeOnMatrices<double>(const_cast<const Matrix *>(matrixInverse), const_cast<const Matrix *>(BMatrix));
  Expression * exactSolutions[n];
  for (int i = 0; i < n; i++) {
    Expression * sol = result->matrixOperand(i,0);
    exactSolutions[i] = sol;
    result->detachOperand(sol);
    Expression::Simplify(&exactSolutions[i], *context, angleUnit);
    assert(exactSolutions[i] != nullptr);
    solutions[i] = exactSolutions[i]->approximateToScalar<double>(*context, angleUnit);
    delete exactSolutions[i];
  }
  delete result;
  delete matrixInverse;
  delete BMatrix;
  delete AMatrix;
}

}
