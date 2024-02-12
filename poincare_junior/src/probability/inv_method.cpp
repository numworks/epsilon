#include "inv_method.h"

#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/rational.h>

namespace PoincareJ {

bool InverseMethod::shallowReduce(const Tree **x,
                                  const Distribution *distribution,
                                  const Tree **parameters,
                                  Tree *expression) const {
  const Tree *a = x[0];
  // Check a
  if (!a->isRational()) {
    return false;
  }

  // Special values

  // Undef if a < 0 or a > 1
  if (Rational::Sign(a).isStrictlyNegative()) {
    expression->cloneTreeOverTree(KUndef);
    return true;
  }
  if (Rational::IsGreaterThanOne(a)) {
    expression->cloneTreeOverTree(KUndef);
    return true;
  }

  bool is0 = a->isZero();
  bool is1 = !is0 && a->isOne();

  if (is0 || is1) {
    // TODO: for all distributions with finite support
    if (distribution->hasType(Distribution::Type::Binomial)) {
      if (is0) {
        const Tree *p = parameters[1];
        if (!p->isRational()) {
          return false;
        }
        if (p->isOne()) {
          expression->cloneTreeOverTree(0_e);
          return true;
        }
        expression->cloneTreeOverTree(KUndef);
        return true;
      }
      // n if a == 1 (TODO: false if p == 0 ?)
      const Tree *n = parameters[0];
      expression->cloneTreeOverTree(n);
      return true;
    }

    if (distribution->hasType(Distribution::Type::Geometric)) {
      if (is0) {
        expression->cloneTreeOverTree(KUndef);
        return true;
      }

      // is1
      const Tree *p = parameters[0];
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

    if (distribution->hasType(Distribution::Type::Normal) ||
        distribution->hasType(Distribution::Type::Student)) {
      // Normal and Student (all distributions with real line support)
      expression->cloneTreeOverTree(is0 ? KMult(-1_e, KInf) : KInf);
      return true;
    }
  }

  // expectedValue if a == 0.5 and continuous and symmetrical
  if (a->isHalf()) {
    if (distribution->hasType(Distribution::Type::Normal)) {
      const Tree *mu = parameters[0];
      expression->cloneTreeOverTree(mu);
      return true;
    }
    if (distribution->hasType(Distribution::Type::Student)) {
      expression->cloneTreeOverTree(0_e);
      return true;
    }
  }

  return false;
}

}  // namespace PoincareJ
