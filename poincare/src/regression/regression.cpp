#include "regression.h"

#include <omg/float.h>
#include <poincare/expression.h>
#include <poincare/layout.h>
#include <poincare/numeric/solver.h>
#include <poincare/old/symbol.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/numeric/helpers.h>

#include <cmath>

using namespace Poincare::Internal;

namespace Poincare::Regression {

Layout Regression::templateLayout() const {
  const char* layoutString = formula() + sizeof("y=") - 1;
  return Layout::String(layoutString);
}

Layout Regression::equationLayout(
    const double* modelCoefficients, const char* ySymbol, int significantDigits,
    Poincare::Preferences::PrintFloatMode displayMode) const {
  UserExpression formula = expression(modelCoefficients);
  if (formula.isUninitialized()) {
    return Layout();
  }
  UserExpression equation = UserExpression::Create(
      KEqual(KA, KB),
      {.KA = Symbol::Builder(ySymbol, strlen(ySymbol)), .KB = formula});
  return equation.createLayout(displayMode, significantDigits, nullptr);
}

Poincare::UserExpression Regression::expression(
    const double* modelCoefficients) const {
  for (int i = 0; i < numberOfCoefficients(); i++) {
    if (std::isnan(modelCoefficients[i])) {
      return UserExpression();
    }
  }
  return privateExpression(modelCoefficients);
}

double Regression::levelSet(const double* modelCoefficients, double xMin,
                            double xMax, double y,
                            Poincare::Context* context) const {
  UserExpression e = expression(modelCoefficients);
  if (e.isUninitialized()) {
    return NAN;
  }
  Tree* yTree = Internal::SharedTreeStack->pushFloat(y);
  // TODO: use y+evaluate() instead of yTree+e in nextIntersection
  double result = Poincare::Internal::Solver(xMin, xMax, context)
                      .nextIntersection(yTree, e)
                      .x();
  yTree->removeTree();
  return result;
}

void Regression::fit(const Series* series, double* modelCoefficients,
                     Poincare::Context* context) const {
  if (!dataSuitableForFit(series)) {
    initCoefficientsForFit(modelCoefficients, NAN, true);
    return;
  }
  privateFit(series, modelCoefficients, context);
}

void Regression::privateFit(const Series* series, double* modelCoefficients,
                            Poincare::Context* context) const {
  initCoefficientsForFit(modelCoefficients, k_initialCoefficientValue, false,
                         series);
  fitLevenbergMarquardt(series, modelCoefficients, context);
  uniformizeCoefficientsFromFit(modelCoefficients);
}

bool Regression::dataSuitableForFit(const Series* series) const {
  return series->numberOfDistinctAbscissaeGreaterOrEqualTo(
      numberOfCoefficients());
}

void Regression::fitLevenbergMarquardt(const Series* series,
                                       double* modelCoefficients,
                                       Context* context) const {
  /* We want to find the best coefficients of the regression to minimize the sum
   * of the squares of the difference between a data point and the corresponding
   * point of the fitting regression (chi2 function).
   * We use the Levenberg-Marquardt algorithm to minimize this chi2 merit
   * function.
   * The equation to solve is A'*da = B, with A' a damped version of the chi2
   * Hessian matrix, da the coefficients increments and B colinear to the
   * gradient of chi2.*/
  double currentChi2 = chi2(series, modelCoefficients);
  double lambda = k_initialLambda;
  int n = numberOfCoefficients();  // n unknown coefficients
  int smallChi2ChangeCounts = 0;
  int iterationCount = 0;
  while (smallChi2ChangeCounts < k_consecutiveSmallChi2ChangesLimit &&
         iterationCount < k_maxIterations) {
    // Create the alpha prime matrix (it is symmetric)
    double coefficientsAPrime[Regression::k_maxNumberOfCoefficients *
                              Regression::k_maxNumberOfCoefficients];
    assert(n > 0);  // Ensure that coefficientsAPrime is initialized
    for (int i = 0; i < n; i++) {
      for (int j = i; j < n; j++) {
        double alphaPrime =
            alphaPrimeCoefficient(series, modelCoefficients, i, j, lambda);
        coefficientsAPrime[i * n + j] = alphaPrime;
        if (i != j) {
          coefficientsAPrime[j * n + i] = alphaPrime;
        }
      }
    }
    // Create the beta matrix
    double operandsB[Regression::k_maxNumberOfCoefficients];
    for (int j = 0; j < n; j++) {
      operandsB[j] = betaCoefficient(series, modelCoefficients, j);
    }

    // Compute the equation solution (= vector of coefficients increments)
    double modelCoefficientSteps[Regression::k_maxNumberOfCoefficients];
    if (solveLinearSystem(modelCoefficientSteps, coefficientsAPrime, operandsB,
                          n, context) < 0) {
      break;
    }

    // Compute the new coefficients
    double newModelCoefficients[Regression::k_maxNumberOfCoefficients];
    for (int i = 0; i < n; i++) {
      newModelCoefficients[i] = modelCoefficients[i] + modelCoefficientSteps[i];
    }

    // Compare new chi2 with the previous value
    double newChi2 = chi2(series, newModelCoefficients);
    smallChi2ChangeCounts =
        (fabs(currentChi2 - newChi2) > k_chi2ChangeCondition)
            ? 0
            : smallChi2ChangeCounts + 1;
    if (newChi2 >= currentChi2) {
      lambda *= k_lambdaFactor;
    } else {
      lambda /= k_lambdaFactor;
      for (int i = 0; i < n; i++) {
        modelCoefficients[i] = newModelCoefficients[i];
      }
      currentChi2 = newChi2;
    }
    iterationCount++;
  }
}

double Regression::chi2(const Series* series,
                        const double* modelCoefficients) const {
  double result = 0.0;
  for (int n = series->numberOfPairs(), i = 0; i < n; i++) {
    double xi = series->getX(i);
    double yi = series->getY(i);
    double difference = yi - evaluate(modelCoefficients, xi);
    result += difference * difference;
  }
  return result;
}

/* a'(k,k) = a(k,k) * (1 + lambda)
 * a'(k,l) = a(l,k) when (k != l) */
double Regression::alphaPrimeCoefficient(const Series* series,
                                         const double* modelCoefficients, int k,
                                         int l, double lambda) const {
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
    result = alphaCoefficient(series, modelCoefficients, k, l) * (1.0 + lambda);
    if (std::fabs(result) < OMG::Float::EpsilonLax<double>()) {
      result = 2 * OMG::Float::EpsilonLax<double>();
    }
  } else {
    result = alphaCoefficient(series, modelCoefficients, l, k);
  }
  return result;
}

// a(k,l) = sum(0, N-1, derivate(y(xi|a), ak) * derivate(y(xi|a), a))
double Regression::alphaCoefficient(const Series* series,
                                    const double* modelCoefficients, int k,
                                    int l) const {
  assert(k >= 0 && k < numberOfCoefficients());
  assert(l >= 0 && l < numberOfCoefficients());
  double result = 0.0;
  for (int n = series->numberOfPairs(), i = 0; i < n; i++) {
    double xi = series->getX(i);
    result += partialDerivate(modelCoefficients, k, xi) *
              partialDerivate(modelCoefficients, l, xi);
  }
  return result;
}

// b(k) = sum(0, N-1, (yi - y(xi|a)) * derivate(y(xi|a), ak))
double Regression::betaCoefficient(const Series* series,
                                   const double* modelCoefficients,
                                   int k) const {
  assert(k >= 0 && k < numberOfCoefficients());
  double result = 0.0;
  for (int n = series->numberOfPairs(), i = 0; i < n; i++) {
    double xi = series->getX(i);
    double yi = series->getY(i);
    result += (yi - evaluate(modelCoefficients, xi)) *
              partialDerivate(modelCoefficients, k, xi);
  }
  return result;
}

int Regression::solveLinearSystem(double* solutions, double* coefficients,
                                  double* constants, int solutionDimension,
                                  Context* context) const {
  int n = solutionDimension;
  assert(n <= k_maxNumberOfCoefficients);
  double
      coefficientsSave[k_maxNumberOfCoefficients * k_maxNumberOfCoefficients];
  for (int i = 0; i < n * n; i++) {
    coefficientsSave[i] = coefficients[i];
  }
  int inverseResult = Matrix::ArrayInverse(coefficients, n, n);
  int numberOfMatrixModifications = 0;
  while (inverseResult < 0 &&
         numberOfMatrixModifications < k_maxMatrixInversionFixIterations) {
    /* If the matrix is not invertible, we modify it to try to make
     * it invertible by multiplying the diagonal coefficients by 1+i/n. This
     * will change the iterative path of the algorithm towards the chi2 minimum,
     * but not the final solution itself, as the stopping condition is that chi2
     * is at its minimum, so when B is null. */
    for (int i = 0; i < n; i++) {
      coefficientsSave[i * n + i] =
          (1 + ((double)i) / ((double)n)) * coefficientsSave[i * n + i];
    }
    inverseResult = Matrix::ArrayInverse(coefficientsSave, n, n);
    numberOfMatrixModifications++;
  }
  if (inverseResult < 0) {
    return -1;
  }
  if (numberOfMatrixModifications > 0) {
    for (int i = 0; i < n * n; i++) {
      coefficients[i] = coefficientsSave[i];
    }
  }
  MatrixMultiplicationOnArrays<double>(coefficients, constants, solutions, n, n,
                                       1);
  return 0;
}

void Regression::initCoefficientsForFit(double* modelCoefficients,
                                        double defaultValue,
                                        bool forceDefaultValue,
                                        const Series* series) const {
  if (forceDefaultValue) {
    Regression::specializedInitCoefficientsForFit(modelCoefficients,
                                                  defaultValue);
  } else {
    specializedInitCoefficientsForFit(modelCoefficients, defaultValue, series);
  }
}

void Regression::specializedInitCoefficientsForFit(double* modelCoefficients,
                                                   double defaultValue,
                                                   const Series* series) const {
  const int nbCoef = numberOfCoefficients();
  for (int i = 0; i < nbCoef; i++) {
    modelCoefficients[i] = defaultValue;
  }
}

}  // namespace Poincare::Regression
