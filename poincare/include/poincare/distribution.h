#ifndef POINCARE_DISTRIBUTION_H
#define POINCARE_DISTRIBUTION_H

#include <poincare/solver.h>

namespace Poincare {

class Distribution {
public:
  enum class Type : uint8_t{
    // Order matters (used as displayed order in apps/inference)
    Binomial,
    Uniform,
    Exponential,
    Normal,
    ChiSquared,
    Student,
    Geometric,
    Poisson,
    Fisher
  };

  static constexpr int k_maxNumberOfParameters = 2;
  static constexpr int numberOfParameters(Type type) {
    switch (type) {
    case Type::Student:
    case Type::Poisson:
    case Type::Geometric:
    case Type::Exponential:
    case Type::ChiSquared:
      return 1;
    case Type::Uniform:
    case Type::Fisher:
    case Type::Normal:
    case Type::Binomial:
      return 2;
    }
  }

  static const Distribution * Get(Type type);

  virtual Type type() const = 0;
  bool hasType(Type type) const {
    /* assumes no distribution has been constructed outside of Get which is
     * enforced by the protected constructor */
    return this == Get(type);
  }

  virtual bool isContinuous() const = 0;
  virtual bool isSymmetrical() const = 0;
  virtual double meanAbscissa() { assert(false); return NAN; } // Must be implemented by all symmetrical and continouous distributions.

  virtual float EvaluateAtAbscissa(float x, const float * parameters) const = 0;
  virtual double EvaluateAtAbscissa(double x, const double * parameters) const = 0;

  virtual float CumulativeDistributiveFunctionAtAbscissa(float x, const float * parameters) const = 0;
  virtual double CumulativeDistributiveFunctionAtAbscissa(double x, const double * parameters) const = 0;

  virtual float CumulativeDistributiveInverseForProbability(float x, const float * parameters) const = 0;
  virtual double CumulativeDistributiveInverseForProbability(double x, const double * parameters) const = 0;

  virtual float CumulativeDistributiveFunctionForRange(float x, float y, const float * parameters) const = 0;
  virtual double CumulativeDistributiveFunctionForRange(double x, double y, const double * parameters) const = 0;

  virtual bool ParametersAreOK(const float * parameters) const = 0;
  virtual bool ParametersAreOK(const double * parameters) const = 0;

  /* ExpressionParametersAreOK returns true if the expression could be verified.
   * The result of the verification is *result. */
  virtual bool ExpressionParametersAreOK(bool * result, const Expression * parameters, Context * context) const = 0;

protected:
  constexpr Distribution() {}
  /* This method looks for bounds such that:
   * cumulativeDistributionEvaluation(xmin) < 0 < cumulativeDistributionEvaluation(xmax)
   */
  template <typename T> static void findBoundsForBinarySearch(Solver::ValueAtAbscissa cumulativeDistributionEvaluation, Context * context, const void * auxiliary, T & xmin, T & xmax);
  double cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(double p, double ax, double bx, double * parameters) const;


};

}

#endif
