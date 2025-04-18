#include "trigonometry_exact_formulas.h"

#include <poincare/src/memory/pattern_matching.h>

#include "k_tree.h"

namespace Poincare::Internal {

constexpr ExactFormula ExactFormulas[] = {
    /* Angles in [0, π/4] */
    // θ, cos(θ), sin(θ), tan(θ)
    // 0, 1, 0, 0
    {0_e, 1_e, 0_e, 0_e},
    // π/4, √2/2, √2/2, 1
    {KMult(1_e / 4_e, π_e), KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(2_e)))),
     KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(2_e)))), 1_e},
    // π/5, (1+√5)/4, √((5-√5)/8), √(5-2√5)
    {KMult(1_e / 5_e, π_e),
     KMult(1_e / 4_e, KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))),
     KExp(KMult(
         1_e / 2_e,
         KLn(KMult(1_e / 8_e,
                   KAdd(5_e, KMult(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))),
     KExp(
         KMult(1_e / 2_e,
               KLn(KAdd(5_e, KMult(-2_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))},
    // π/6, √3/2, 1/2, √3/3
    {KMult(1_e / 6_e, π_e), KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(3_e)))),
     1_e / 2_e, KMult(1_e / 3_e, KExp(KMult(1_e / 2_e, KLn(3_e))))},
    // π/8, √(2+√2)/2, √(2-√2)/2, -1+√2
    {KMult(1_e / 8_e, π_e),
     KMult(1_e / 2_e,
           KExp(KMult(1_e / 2_e,
                      KLn(KAdd(2_e, KExp(KMult(1_e / 2_e, KLn(2_e)))))))),
     KMult(1_e / 2_e,
           KExp(KMult(
               1_e / 2_e,
               KLn(KAdd(2_e, KMult(-1_e, KExp(KMult(1_e / 2_e, KLn(2_e))))))))),
     KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(2_e))))},
    // π/10, √((5+√5)/8), (√5-1)/4, √(1-2√5/5)
    {KMult(1_e / 10_e, π_e),
     KExp(KMult(
         1_e / 2_e,
         KLn(KMult(1_e / 8_e, KAdd(5_e, KExp(KMult(1_e / 2_e, KLn(5_e)))))))),
     KMult(1_e / 4_e, KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))),
     KExp(KMult(
         1_e / 2_e,
         KLn(KAdd(1_e, KMult(-2_e / 5_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))},
    // π/12, 1/4×√2×(1+√3), 1/4×√2×(-1+√3), 2-√3
    {KMult(1_e / 12_e, π_e),
     KMult(1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(3_e))))),
     KMult(1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(3_e))))),
     KAdd(2_e, KMult(-1_e, KExp(KMult(1_e / 2_e, KLn(3_e)))))},
    /* Angles in ]π/4, π/2]
     * TODO : Remove them with asin(x) = π/2 - acos(x) and
     *        atan(x) = π/2 - atan(1/x) advanced reduction.
     */
    // π/2, 0, 1, undef
    {KMult(1_e / 2_e, π_e), 0_e, 1_e, KUndef},
    // π/3, 1/2, √3/2, √3
    {KMult(1_e / 3_e, π_e), 1_e / 2_e,
     KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(3_e)))),
     KExp(KMult(1_e / 2_e, KLn(3_e)))},
    // 3π/10, √((5-√5)/8), (1+√5)/4, √(1+2√5/5)
    {KMult(3_e / 10_e, π_e),
     KExp(KMult(
         1_e / 2_e,
         KLn(KMult(1_e / 8_e,
                   KAdd(5_e, KMult(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))),
     KMult(1_e / 4_e, KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))),
     KExp(KMult(
         1_e / 2_e,
         KLn(KAdd(1_e, KMult(2_e / 5_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))},
    // 3π/8, √(2-√2)/2, √(2+√2)/2, 1+√2
    {KMult(3_e / 8_e, π_e),
     KMult(1_e / 2_e,
           KExp(KMult(
               1_e / 2_e,
               KLn(KAdd(2_e, KMult(-1_e, KExp(KMult(1_e / 2_e, KLn(2_e))))))))),
     KMult(1_e / 2_e,
           KExp(KMult(1_e / 2_e,
                      KLn(KAdd(2_e, KExp(KMult(1_e / 2_e, KLn(2_e)))))))),
     KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(2_e))))},
    // 2π/5, (√5-1)/4, √((5+√5)/8), √(5+2√5)
    {KMult(2_e / 5_e, π_e),
     KMult(1_e / 4_e, KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))),
     KExp(KMult(
         1_e / 2_e,
         KLn(KMult(1_e / 8_e, KAdd(5_e, KExp(KMult(1_e / 2_e, KLn(5_e)))))))),
     KExp(KMult(1_e / 2_e,
                KLn(KAdd(5_e, KMult(2_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))},
    // 5π/12, 1/4×√2×(-1+√3), 1/4×√2×(1+√3), 2+√3
    {KMult(5_e / 12_e, π_e),
     KMult(1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(3_e))))),
     KMult(1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(3_e))))),
     KAdd(2_e, KExp(KMult(1_e / 2_e, KLn(3_e))))},
};

ExactFormula ExactFormula::GetExactFormulaAtIndex(int n) {
  assert(n >= 0 && n < k_totalNumberOfFormula);
  return ExactFormulas[n];
}

const Tree* ExactFormula::GetTrigOf(const Tree* angle, Type type) {
  assert(type == Type::Sin || type == Type::Cos || type == Type::Tan);
  for (int i = 0; i < k_numberOfFormulaForTrig; i++) {
    ExactFormula ef = GetExactFormulaAtIndex(i);
    if (angle->treeIsIdenticalTo(ef.m_angle)) {
      const Tree* result = type == Type::Sin   ? ef.m_sin
                           : type == Type::Cos ? ef.m_cos
                                               : ef.m_tan;
      assert(!result->isUndef());
      return result;
    }
  }
  return nullptr;
}

const Tree* ExactFormula::GetAngleOf(const Tree* trig, Type type) {
  assert(type == Type::Sin || type == Type::Cos || type == Type::Tan);
  for (int i = 0; i < k_totalNumberOfFormula; i++) {
    ExactFormula ef = GetExactFormulaAtIndex(i);
    const Tree* treeToMatch = type == Type::Sin   ? ef.m_sin
                              : type == Type::Cos ? ef.m_cos
                                                  : ef.m_tan;
    if (!treeToMatch->isUndef() && trig->treeIsIdenticalTo(treeToMatch)) {
      return ef.m_angle;
    }
  }
  return nullptr;
}

}  // namespace Poincare::Internal
