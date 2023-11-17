#include "logarithm.h"

#include <poincare_junior/src/expression/arithmetic.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

bool Logarithm::ExpandLnOnRational(Tree* e) {
  if (!e->isLn() || !e->child(0)->isRational()) {
    return false;
  }
  const Tree* child = e->child(0);
  Tree* denominator =
      child->isInteger()
          ? nullptr
          : ExpandLnOnInteger(Rational::Denominator(child), false);
  Tree* numerator =
      ExpandLnOnInteger(Rational::Numerator(child), denominator == nullptr);
  if (!numerator) {
    assert(!denominator);
    // ln(13) -> ln(13)
    return false;
  }
  Tree* result;
  if (denominator) {
    // ln(13/11) -> ln(13)-ln(11)
    PatternMatching::CreateAndSimplify(KAdd(KA, KMult(-1_e, KB)),
                                       {.KA = numerator, .KB = denominator});
    numerator->removeTree();
    denominator->removeTree();
    // denominator is now KAdd(KA, KMult(-1_e, KB)
    result = denominator;
  } else {
    // ln(12) -> 2ln(2)+ln(3)
    result = numerator;
  }
  e->moveTreeOverTree(result);
  return true;
}

Tree* Logarithm::ExpandLnOnInteger(IntegerHandler m, bool escapeIfPrime) {
  Arithmetic::FactorizedInteger factorization =
      Arithmetic::PrimeFactorization(m);
  if (escapeIfPrime && (factorization.numberOfFactors == 0 ||
                        (factorization.numberOfFactors == 1 &&
                         factorization.coefficients[0] == 1))) {
    return nullptr;
  }
  Tree* result = KAdd.node<0>->cloneNode();
  for (int i = 0; i < factorization.numberOfFactors; i++) {
    if (factorization.coefficients[i] > 1) {
      KMult.node<2>->cloneNode();
      Integer::Push(factorization.coefficients[i]);
    }
    KLn->cloneNode();
    Integer::Push(factorization.factors[i]);
  }
  NAry::SetNumberOfChildren(result, factorization.numberOfFactors);
  NAry::SquashIfPossible(result);
  assert(!Simplification::DeepSystematicReduce(result));
  return result;
}

}  // namespace PoincareJ
