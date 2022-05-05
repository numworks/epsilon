#ifndef POINCARE_DISTRIBUTION_H
#define POINCARE_DISTRIBUTION_H

#include <poincare/solver.h>

namespace Poincare {

class Distribution {
public:
  virtual float EvaluateAtAbscissa(float x, const float * parameters) = 0;
  virtual double EvaluateAtAbscissa(double x, const double * parameters) = 0;

  virtual float CumulativeDistributiveFunctionAtAbscissa(float x, const float * parameters) = 0;
  virtual double CumulativeDistributiveFunctionAtAbscissa(double x, const double * parameters) = 0;

  virtual float CumulativeDistributiveInverseForProbability(float x, const float * parameters) = 0;
  virtual double CumulativeDistributiveInverseForProbability(double x, const double * parameters) = 0;

  virtual bool ParametersAreOK(const float * parameters) = 0;
  virtual bool ParametersAreOK(const double * parameters) = 0;

  /* ExpressionParametersAreOK returns true if the expression could be verified.
   * The result of the verification is *result. */
  virtual bool ExpressionParametersAreOK(bool * result, const Expression * parameters, Context * context) = 0;

protected:
  /* This method looks for bounds such that:
   * cumulativeDistributionEvaluation(xmin) < 0 < cumulativeDistributionEvaluation(xmax)
   */
  template <typename T> static void findBoundsForBinarySearch(Poincare::Solver::ValueAtAbscissa cumulativeDistributionEvaluation, Poincare::Context * context, const void * auxiliary, T & xmin, T & xmax);
};

}

#endif
