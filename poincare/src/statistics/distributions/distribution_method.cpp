#include "distribution_method.h"

#include <poincare/src/expression/infinity.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/rational.h>
#include <poincare/statistics/distribution.h>

namespace Poincare::Internal::DistributionMethod {

template <typename T>
T EvaluateAtAbscissa(Type method, const Abscissae<T> x,
                     Distribution::Type distribType,
                     const Distribution::ParametersArray<T> parameters) {
  switch (method) {
    case Type::PDF:
      return Distribution::EvaluateAtAbscissa(distribType, x[0], parameters);
    case Type::CDF:
      return Distribution::CumulativeDistributiveFunctionAtAbscissa(
          distribType, x[0], parameters);
    case Type::CDFRange:
      return Distribution::CumulativeDistributiveFunctionForRange(
          distribType, x[0], x[1], parameters);
    case Type::Inverse:
      return Distribution::CumulativeDistributiveInverseForProbability(
          distribType, x[0], parameters);
    default:
      OMG::unreachable();
  }
}

bool shallowReducePDF(
    const DistributionMethod::Abscissae<const Tree*> abscissae,
    Distribution::Type distribType,
    const Distribution::ParametersArray<const Tree*> parameters,
    Tree* expression) {
  const Tree* x = abscissae[0];

  if (Infinity::IsPlusOrMinusInfinity(x)) {
    expression->cloneTreeOverTree(0_e);
    return true;
  }

  if (!x->isRational()) {
    return false;
  }

  if (Rational::Sign(x).isStrictlyNegative() &&
      (distribType == Distribution::Type::Binomial ||
       distribType == Distribution::Type::Poisson ||
       distribType == Distribution::Type::Geometric ||
       distribType == Distribution::Type::Hypergeometric)) {
    expression->cloneTreeOverTree(0_e);
    return true;
  }

  if (!Distribution::IsContinuous(distribType) && !x->isInteger()) {
    Tree* floorX = IntegerHandler::Quotient(Rational::Numerator(x),
                                            Rational::Denominator(x));
    // Replacing x in expression by its floor
    assert(x == expression->child(0));
    expression->child(0)->moveTreeOverTree(floorX);
    return true;
  }

  return false;
}

bool shallowReduceCDF(
    const DistributionMethod::Abscissae<const Tree*> abscissae,
    Distribution::Type distribType,
    const Distribution::ParametersArray<const Tree*> parameters,
    Tree* expression) {
  const Tree* x = abscissae[0];

  if (x->isInf()) {
    expression->cloneTreeOverTree(1_e);
    return true;
  }
  if (Infinity::IsMinusInfinity(x)) {
    expression->cloneTreeOverTree(0_e);
    return true;
  }

  return false;
}

bool shallowReduceCDFRange(
    const DistributionMethod::Abscissae<const Tree*> abscissae,
    Distribution::Type distribType,
    const Distribution::ParametersArray<const Tree*> parameters,
    Tree* expression) {
  const Tree* x = abscissae[0];
  const Tree* y = abscissae[1];

  if (Infinity::IsMinusInfinity(x)) {
    if (y->isInf()) {
      expression->cloneTreeOverTree(1_e);
      return true;
    }
    if (Infinity::IsMinusInfinity(y)) {
      expression->cloneTreeOverTree(0_e);
      return true;
    }
    /* TODO: return CDF of the same distributions with the same parameters
     * tcdfrange(-inf, 4, 5) => tcdf(4, 5) */
  }

  return false;
}

bool shallowReduceInverse(
    const DistributionMethod::Abscissae<const Tree*> abscissae,
    Distribution::Type distribType,
    const Distribution::ParametersArray<const Tree*> parameters,
    Tree* expression) {
  const Tree* a = abscissae[0];
  // Check a
  if (!a->isRational()) {
    return false;
  }

  // Special values

  // Undef if a < 0 or a > 1
  if (Rational::Sign(a).isStrictlyNegative()) {
    expression->cloneTreeOverTree(KOutOfDefinition);
    return true;
  }
  if (Rational::IsGreaterThanOne(a)) {
    expression->cloneTreeOverTree(KOutOfDefinition);
    return true;
  }

  bool is0 = a->isZero();
  bool is1 = !is0 && a->isOne();

  if (is0 || is1) {
    // TODO: for all distributions with finite support
    if (distribType == Distribution::Type::Binomial) {
      if (is0) {
        const Tree* p = parameters[1];
        if (!p->isRational()) {
          return false;
        }
        if (p->isOne()) {
          expression->cloneTreeOverTree(0_e);
          return true;
        }
        expression->cloneTreeOverTree(KOutOfDefinition);
        return true;
      }
      // n if a == 1 (TODO: false if p == 0 ?)
      const Tree* n = parameters[0];
      expression->cloneTreeOverTree(n);
      return true;
    }

    if (distribType == Distribution::Type::Geometric) {
      if (is0) {
        expression->cloneTreeOverTree(KOutOfDefinition);
        return true;
      }

      // is1
      const Tree* p = parameters[0];
      if (!p->isRational()) {
        return false;
      }
      if (p->isOne()) {
        expression->cloneTreeOverTree(1_e);
        return true;
      }
      expression->cloneTreeOverTree(KInf);
      return true;
    }

    if (distribType == Distribution::Type::Normal ||
        distribType == Distribution::Type::Student) {
      // Normal and Student (all distributions with real line support)
      expression->cloneTreeOverTree(is0 ? KMult(-1_e, KInf) : KInf);
      return true;
    }
  }

  // expectedValue if a == 0.5 and continuous and symmetrical
  if (a->isHalf()) {
    if (distribType == Distribution::Type::Normal) {
      const Tree* mu = parameters[0];
      expression->cloneTreeOverTree(mu);
      return true;
    }
    if (distribType == Distribution::Type::Student) {
      expression->cloneTreeOverTree(0_e);
      return true;
    }
  }

  return false;
}

bool ShallowReduce(Type method, const Abscissae<const Tree*> abscissae,
                   Distribution::Type distribType,
                   const Distribution::ParametersArray<const Tree*> parameters,
                   Tree* expression) {
  switch (method) {
    case Type::PDF:
      return shallowReducePDF(abscissae, distribType, parameters, expression);
    case Type::CDF:
      return shallowReduceCDF(abscissae, distribType, parameters, expression);
    case Type::CDFRange:
      return shallowReduceCDFRange(abscissae, distribType, parameters,
                                   expression);
    case Type::Inverse:
      return shallowReduceInverse(abscissae, distribType, parameters,
                                  expression);
    default:
      OMG::unreachable();
  }
}

template float EvaluateAtAbscissa(
    Type method, const Abscissae<float> x, Distribution::Type distribType,
    const Distribution::ParametersArray<float> parameters);
template double EvaluateAtAbscissa(
    Type method, const Abscissae<double> x, Distribution::Type distribType,
    const Distribution::ParametersArray<double> parameters);

}  // namespace Poincare::Internal::DistributionMethod
