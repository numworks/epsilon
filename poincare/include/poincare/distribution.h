#ifndef POINCARE_DISTRIBUTION_H
#define POINCARE_DISTRIBUTION_H

#include <poincare/solver_algorithms.h>

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
    Hypergeometric,
    Poisson,
    Fisher,
  };

  constexpr static int k_maxNumberOfParameters = 3;
  constexpr static int numberOfParameters(Type type) {
    switch (type) {
    case Type::Student:
    case Type::Poisson:
    case Type::Geometric:
    case Type::Exponential:
    case Type::ChiSquared:
      return 1;
    case Type::Hypergeometric:
      return 3;
    default:
       assert(type == Type::Binomial || type == Type::Uniform || type == Type::Fisher || type == Type::Normal);
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

  virtual float evaluateAtAbscissa(float x, const float * parameters) const = 0;
  virtual double evaluateAtAbscissa(double x, const double * parameters) const = 0;

  virtual float cumulativeDistributiveFunctionAtAbscissa(float x, const float * parameters) const = 0;
  virtual double cumulativeDistributiveFunctionAtAbscissa(double x, const double * parameters) const = 0;

  virtual float cumulativeDistributiveInverseForProbability(float x, const float * parameters) const = 0;
  virtual double cumulativeDistributiveInverseForProbability(double x, const double * parameters) const = 0;

  virtual float cumulativeDistributiveFunctionForRange(float x, float y, const float * parameters) const = 0;
  virtual double cumulativeDistributiveFunctionForRange(double x, double y, const double * parameters) const = 0;

  virtual bool parametersAreOK(const float * parameters) const = 0;
  virtual bool parametersAreOK(const double * parameters) const = 0;

  /* expressionParametersAreOK returns true if the expression could be verified.
   * The result of the verification is *result. */
  virtual bool expressionParametersAreOK(bool * result, const Expression * parameters, Context * context) const = 0;

protected:
  constexpr Distribution() {}
  /* This method looks for bounds such that:
   * cumulativeDistributionEvaluation(xmin) < 0 < cumulativeDistributionEvaluation(xmax)
   */
  template <typename T> static void findBoundsForBinarySearch(typename Solver<T>::FunctionEvaluation cumulativeDistributionEvaluation, const void * auxiliary, T & xmin, T & xmax);
  double cumulativeDistributiveInverseForProbabilityUsingIncreasingFunctionRoot(double p, double ax, double bx, double * parameters) const;


};

}

#endif
