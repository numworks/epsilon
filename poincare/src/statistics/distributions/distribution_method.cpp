#include "distribution_method.h"

#include <poincare/src/expression/infinity.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/rational.h>

namespace Poincare::Internal {

bool shallowReducePDF(const Tree** abscissae, Distribution distribution,
                      const Tree** parameters, Tree* expression) {
  const Tree* x = abscissae[0];

  if (Infinity::IsPlusOrMinusInfinity(x)) {
    expression->cloneTreeOverTree(0_e);
    return true;
  }

  if (!x->isRational()) {
    return false;
  }

  if (Rational::Sign(x).isStrictlyNegative() &&
      (distribution.type() == Distribution::Type::Binomial ||
       distribution.type() == Distribution::Type::Poisson ||
       distribution.type() == Distribution::Type::Geometric ||
       distribution.type() == Distribution::Type::Hypergeometric)) {
    expression->cloneTreeOverTree(0_e);
    return true;
  }

  if (!distribution.isContinuous() && !x->isInteger()) {
    Tree* floorX = IntegerHandler::Quotient(Rational::Numerator(x),
                                            Rational::Denominator(x));
    // Replacing x in expression by its floor
    assert(x == expression->child(0));
    expression->child(0)->moveTreeOverTree(floorX);
    return true;
  }

  return false;
}

bool shallowReduceCDF(const Tree** abscissae, Distribution distribution,
                      const Tree** parameters, Tree* expression) {
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

bool shallowReduceCDFRange(const Tree** abscissae, Distribution distribution,
                           const Tree** parameters, Tree* expression) {
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

bool shallowReduceInverse(const Tree** x, Distribution distribution,
                          const Tree** parameters, Tree* expression) {
  const Tree* a = x[0];
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
    if (distribution.type() == Distribution::Type::Binomial) {
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

    if (distribution.type() == Distribution::Type::Geometric) {
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

    if (distribution.type() == Distribution::Type::Normal ||
        distribution.type() == Distribution::Type::Student) {
      // Normal and Student (all distributions with real line support)
      expression->cloneTreeOverTree(is0 ? KMult(-1_e, KInf) : KInf);
      return true;
    }
  }

  // expectedValue if a == 0.5 and continuous and symmetrical
  if (a->isHalf()) {
    if (distribution.type() == Distribution::Type::Normal) {
      const Tree* mu = parameters[0];
      expression->cloneTreeOverTree(mu);
      return true;
    }
    if (distribution.type() == Distribution::Type::Student) {
      expression->cloneTreeOverTree(0_e);
      return true;
    }
  }

  return false;
}

bool DistributionMethod::shallowReduce(const Tree** abscissae,
                                       Distribution distribution,
                                       const Tree** parameters,
                                       Tree* expression) const {
  switch (m_type) {
    case Type::PDF:
      return shallowReducePDF(abscissae, distribution, parameters, expression);
    case Type::CDF:
      return shallowReduceCDF(abscissae, distribution, parameters, expression);
    case Type::CDFRange:
      return shallowReduceCDFRange(abscissae, distribution, parameters,
                                   expression);
    case Type::Inverse:
      return shallowReduceInverse(abscissae, distribution, parameters,
                                  expression);
    default:
      OMG::unreachable();
  }
}

}  // namespace Poincare::Internal
