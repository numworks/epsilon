#include "logarithm.h"

#include <poincare_junior/src/expression/arithmetic.h>
#include <poincare_junior/src/expression/k_tree.h>
#include <poincare_junior/src/expression/number.h>
#include <poincare_junior/src/expression/rational.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/exception_checkpoint.h>
#include <poincare_junior/src/memory/pattern_matching.h>
#include <poincare_junior/src/n_ary.h>

namespace PoincareJ {

bool Logarithm::SimplifyLn(Tree* u) {
  Tree* child = u->nextNode();
  if (child->isExponential()) {
    // ln(exp(x)) -> x
    u->removeNode();
    u->removeNode();
    return true;
  }
  if (!child->isInteger()) {
    return false;
  }
  if (child->isMinusOne()) {
    // ln(-1) -> iπ - Necessary so that sqrt(-1)->i
    u->cloneTreeOverTree(KComplex(0_e, π_e));
    return true;
  } else if (child->isOne()) {
    u->cloneTreeOverTree(0_e);
    return true;
  } else if (child->isZero()) {
    ExceptionCheckpoint::Raise(ExceptionType::Nonreal);
  }
  return false;
}

bool Logarithm::ContractLn(Tree* ref) {
  // A? + B*ln(C) + D? = A + ln(C^B) + D if B is an integer.
  PatternMatching::Context ctx;
  if (PatternMatching::Match(KAdd(KTA, KMult(KB, KLn(KC)), KTD), ref, &ctx) &&
      ctx.getNode(KB)->isInteger()) {
    ref->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KAdd(KTA, KLn(KPow(KC, KB)), KTD), ctx));
    return true;
  }
  // A? + ln(B) + ln(C) + D? = A + ln(BC) + D
  return PatternMatching::MatchReplaceAndSimplify(
      ref, KAdd(KTA, KLn(KB), KLn(KC), KTD),
      KAdd(KTA, KLn(KMult(KB, KC)), KTD));
}

bool Logarithm::ExpandLn(Tree* ref) {
  // ln(A*B*...) = ln(A) + ln(B) + ...
  return Simplification::DistributeOverNAry(
      ref, BlockType::Ln, BlockType::Multiplication, BlockType::Addition,
      ExpandSingleChildLn);
}

bool Logarithm::ExpandSingleChildLn(Tree* ref) {
  return
      // ln(12/7) = 2*ln(2) + ln(3) - ln(7)
      ExpandLnOnRational(ref) ||
      // ln(A^B) = B*ln(A)
      PatternMatching::MatchReplaceAndSimplify(ref, KLn(KPow(KA, KB)),
                                               KMult(KB, KLn(KA))) ||
      SimplifyLn(ref);
}

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
