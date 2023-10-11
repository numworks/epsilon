#include "beautification.h"

#include <poincare_junior/src/memory/pattern_matching.h>

#include "simplification.h"

namespace PoincareJ {

bool Beautification::DeepBeautify(Tree* node,
                                  ProjectionContext projectionContext) {
  // Simplification might have unlocked more approximations. Try again.
  bool changed =
      (projectionContext.m_strategy == Strategy::ApproximateToFloat) &&
      Approximation::ApproximateAndReplaceEveryScalar(node);
  if (projectionContext.m_dimension.isUnit()) {
    assert(!projectionContext.m_dimension.unit.vector.isEmpty());
    EditionReference baseUnits;
    if (projectionContext.m_dimension.hasNonKelvinTemperatureUnit()) {
      assert(projectionContext.m_dimension.unit.vector.supportSize() == 1);
      baseUnits = Unit::Push(projectionContext.m_dimension.unit.representative,
                             UnitPrefix::EmptyPrefix());
    } else {
      baseUnits = projectionContext.m_dimension.unit.vector.toBaseUnits();
      Simplification::DeepSystematicReduce(baseUnits);
    }
    node->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KMult(KA, KB), {.KA = node, .KB = baseUnits}));
    baseUnits->removeTree();
    changed = true;
  }
  return Tree::ApplyShallowInDepth(node, ShallowBeautify, &projectionContext) ||
         changed;
}

// Reverse most system projections to display better expressions
bool Beautification::ShallowBeautify(Tree* ref, void* context) {
  ProjectionContext* projectionContext =
      static_cast<ProjectionContext*>(context);
  PoincareJ::AngleUnit angleUnit = projectionContext->m_angleUnit;
  if (ref->type() == BlockType::Trig &&
      angleUnit != PoincareJ::AngleUnit::Radian) {
    Tree* child = ref->child(0);
    child->moveTreeOverTree(PatternMatching::CreateAndSimplify(
        KMult(KA, KB, KPow(π_e, -1_e)),
        {.KA = child,
         .KB = (angleUnit == PoincareJ::AngleUnit::Degree ? 180_e : 200_e)}));
  }

  // PowerReal(A,B) -> A^B
  // PowerMatrix(A,B) -> A^B
  // exp(A? * ln(B) * C?) -> B^(A*C)
  if (PatternMatching::MatchAndReplace(ref, KPowMatrix(KA, KB), KPow(KA, KB)) ||
      PatternMatching::MatchAndReplace(ref, KPowReal(KA, KB), KPow(KA, KB)) ||
      PatternMatching::MatchAndReplace(ref, KExp(KMult(KTA, KLn(KB), KTC)),
                                       KPow(KB, KMult(KTA, KTC)))) {
    // A^0.5 -> Sqrt(A)
    PatternMatching::MatchAndReplace(ref, KPow(KA, KHalf), KSqrt(KA));
    return true;
  }
  bool changed = false;
  // A + B? + (-1)*C + D?-> ((A + B) - C) + D
  // Applied as much as necessary while preserving the order.
  while (PatternMatching::MatchAndReplace(
      ref, KAdd(KA, KTB, KMult(-1_e, KTC), KTD),
      KAdd(KSub(KAdd(KA, KTB), KMult(KTC)), KTD))) {
    changed = true;
  }
  return changed ||
         // Complex(0,1) -> i
         PatternMatching::MatchAndReplace(ref, KComplex(0_e, 1_e), i_e) ||
         // Complex(0,A) -> A*i
         PatternMatching::MatchAndReplace(ref, KComplex(0_e, KA),
                                          KMult(KA, i_e)) ||
         // Complex(A,1) -> A+i
         PatternMatching::MatchAndReplace(ref, KComplex(KA, 1_e),
                                          KAdd(KA, i_e)) ||
         // Complex(A,-1) -> A-i
         PatternMatching::MatchAndReplace(ref, KComplex(KA, -1_e),
                                          KSub(KA, i_e)) ||
         // Complex(A,-B) -> A-B*i
         PatternMatching::MatchAndReplace(ref, KComplex(KA, KMult(-1_e, KTB)),
                                          KSub(KA, KMult(KTB, i_e))) ||
         // Complex(A,B) -> A+B*i
         PatternMatching::MatchAndReplace(ref, KComplex(KA, KB),
                                          KAdd(KA, KMult(KB, i_e))) ||
         // trig(A, 0) -> cos(A)
         PatternMatching::MatchAndReplace(ref, KTrig(KA, 0_e), KCos(KA)) ||
         // trig(A, 1) -> sin(A)
         PatternMatching::MatchAndReplace(ref, KTrig(KA, 1_e), KSin(KA)) ||
         // exp(1) -> e
         PatternMatching::MatchAndReplace(ref, KExp(1_e), e_e) ||
         // exp(A) -> e^A
         PatternMatching::MatchAndReplace(ref, KExp(KA), KPow(e_e, KA)) ||
         // ln(A) * ln(B)^(-1) -> log(A, B)
         PatternMatching::MatchAndReplace(
             ref, KMult(KLn(KA), KPow(KLn(KB), -1_e)), KLogarithm(KA, KB));
}

}  // namespace PoincareJ
