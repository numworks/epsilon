#include "trigonometry_exact_formulas.h"

#include <poincare/src/memory/pattern_matching.h>

#include "k_tree.h"

namespace Poincare::Internal {

ExactFormula ExactFormulas[] = {
    /* Angles in [0, π/4] */
    // θ, cos(θ), sin(θ)
    {0_e, 1_e, 0_e},
    // π/4, √2/2, √2/2
    {KMult(1_e / 4_e, π_e), KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(2_e)))),
     KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(2_e))))},
    // π/5, (1+√5)/4, √((5-√5)/8)
    {KMult(1_e / 5_e, π_e),
     KMult(1_e / 4_e, KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))),
     KExp(KMult(1_e / 2_e,
                KLn(KMult(1_e / 8_e,
                          KAdd(5_e, KMult(-1_e, KExp(KMult(1_e / 2_e,
                                                           KLn(5_e)))))))))},
    // π/6, √3/2, 1/2
    {KMult(1_e / 6_e, π_e), KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(3_e)))),
     1_e / 2_e},
    // π/8, √(2+√2)/2, √(2-√2)/2
    {KMult(1_e / 8_e, π_e),
     KMult(1_e / 2_e,
           KExp(KMult(1_e / 2_e,
                      KLn(KAdd(2_e, KExp(KMult(1_e / 2_e, KLn(2_e)))))))),
     KMult(
         1_e / 2_e,
         KExp(KMult(
             1_e / 2_e,
             KLn(KAdd(2_e, KMult(-1_e, KExp(KMult(1_e / 2_e, KLn(2_e)))))))))},
    // π/10, √((5+√5)/8), (√5-1)/4
    {KMult(1_e / 10_e, π_e),
     KExp(KMult(
         1_e / 2_e,
         KLn(KMult(1_e / 8_e, KAdd(5_e, KExp(KMult(1_e / 2_e, KLn(5_e)))))))),
     KMult(1_e / 4_e, KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e)))))},
    // π/12, 1/4×√2×(1+√3), 1/4×√2×(-1+√3)
    {KMult(1_e / 12_e, π_e),
     KMult(1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(3_e))))),
     KMult(1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(3_e)))))},
    /* Angles in ]π/4, π/2]
     * TODO : Remove them with asin(x) = π/2 - acos(x) advanced reduction. */
    // π/2, 0, 1
    {KMult(1_e / 2_e, π_e), 0_e, 1_e},
    // π/3, 1/2, √3/2
    {KMult(1_e / 3_e, π_e), 1_e / 2_e,
     KMult(1_e / 2_e, KExp(KMult(1_e / 2_e, KLn(3_e))))},
    // 3π/10, √((5-√5)/8), (1+√5)/4
    {KMult(3_e / 10_e, π_e),
     KExp(KMult(
         1_e / 2_e,
         KLn(KMult(1_e / 8_e,
                   KAdd(5_e, KMult(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))),
     KMult(1_e / 4_e, KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(5_e)))))},
    // 3π/8, √(2-√2)/2, √(2+√2)/2
    {KMult(3_e / 8_e, π_e),
     KMult(1_e / 2_e,
           KExp(KMult(
               1_e / 2_e,
               KLn(KAdd(2_e, KMult(-1_e, KExp(KMult(1_e / 2_e, KLn(2_e))))))))),
     KMult(1_e / 2_e,
           KExp(KMult(1_e / 2_e,
                      KLn(KAdd(2_e, KExp(KMult(1_e / 2_e, KLn(2_e))))))))},
    // 2π/5, (√5-1)/4, √((5+√5)/8)
    {KMult(2_e / 5_e, π_e),
     KMult(1_e / 4_e, KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))),
     KExp(KMult(
         1_e / 2_e,
         KLn(KMult(1_e / 8_e, KAdd(5_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))},
    // 5π/12, 1/4×√2×(-1+√3), 1/4×√2×(1+√3)
    {KMult(5_e / 12_e, π_e),
     KMult(1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(3_e))))),
     KMult(1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(3_e)))))},
    /* Additional negative angles :
     * TODO : - Remove them with better sign detection in simplifyATrigOfTrig
     *        - Only one of the two formulas can be matched in each cases */
    // -π/10, -√((5+√5)/8), -(√5-1)/4
    {KMult(-1_e / 10_e, π_e),
     KMult(-1_e, KExp(KMult(
                     1_e / 2_e,
                     KLn(KMult(1_e / 8_e,
                               KAdd(5_e, KExp(KMult(1_e / 2_e, KLn(5_e))))))))),
     KMult(-1_e / 4_e, KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e)))))},
    // -π/12, -1/4×√2×(1+√3), -1/4×√2×(-1+√3)
    {KMult(-1_e / 12_e, π_e),
     KMult(-1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(3_e))))),
     KMult(-1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(3_e)))))},
    // 3π/5, -(√5-1)/4, -√((5+√5)/8)
    {KMult(3_e / 5_e, π_e),
     KMult(-1_e / 4_e, KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(5_e))))),
     KMult(
         -1_e,
         KExp(KMult(1_e / 2_e,
                    KLn(KMult(1_e / 8_e,
                              KAdd(5_e, KExp(KMult(1_e / 2_e, KLn(5_e)))))))))},
    // 7π/12, -1/4×√2×(-1+√3), -1/4×√2×(1+√3)
    {KMult(7_e / 12_e, π_e),
     KMult(-1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(-1_e, KExp(KMult(1_e / 2_e, KLn(3_e))))),
     KMult(-1_e / 4_e, KExp(KMult(1_e / 2_e, KLn(2_e))),
           KAdd(1_e, KExp(KMult(1_e / 2_e, KLn(3_e)))))},
};

ExactFormula ExactFormula::GetExactFormulaAtIndex(int n) {
  assert(n >= 0 && n < k_totalNumberOfFormula);
  return ExactFormulas[n];
}

const Tree* ExactFormula::GetTrigOf(const Tree* angle, const bool isSin) {
  PatternMatching::Context ctx;
  for (int i = 0; i < k_numberOfFormulaForTrig; i++) {
    ExactFormula ef = GetExactFormulaAtIndex(i);
    if (PatternMatching::Match(angle, ef.m_angle, &ctx)) {
      return isSin ? ef.m_sin : ef.m_cos;
    }
  }
  return nullptr;
}

const Tree* ExactFormula::GetAngleOf(const Tree* trig, const bool isAsin) {
  PatternMatching::Context ctx;
  for (int i = 0; i < k_totalNumberOfFormula; i++) {
    ExactFormula ef = GetExactFormulaAtIndex(i);
    if (PatternMatching::Match(trig, isAsin ? ef.m_sin : ef.m_cos, &ctx)) {
      return ef.m_angle;
    }
  }
  return nullptr;
}

}  // namespace Poincare::Internal
