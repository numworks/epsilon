#ifndef POINCARE_EXPRESSION_TRIGONOMETRY_EXACT_FORMULAS_H
#define POINCARE_EXPRESSION_TRIGONOMETRY_EXACT_FORMULAS_H

#include <poincare/src/memory/k_tree.h>
#include <poincare/src/memory/tree.h>

namespace Poincare::Internal {

// Used to test exact formulas
class ExactFormulaTest;

class ExactFormula {
  friend class ExactFormulaTest;

 public:
  template <KTreeConcept T1, KTreeConcept T2, KTreeConcept T3>
  constexpr ExactFormula(T1 angle, T2 cos, T3 sin)
      : m_angle(angle), m_cos(cos), m_sin(sin) {}
  // Find exact formula corresponding to angle, nullptr otherwise
  static const Tree* GetTrigOf(const Tree* angle, const bool isSin);
  // Find exact formula corresponding to trig, nullptr otherwise
  static const Tree* GetAngleOf(const Tree* trig, const bool isAsin);

 private:
  static ExactFormula GetExactFormulaAtIndex(int n);

  constexpr static int k_totalNumberOfFormula = 17;
  // Only formulas for angles in [0, π/4] are used when simplifying Trig
  constexpr static int k_numberOfFormulaForTrig = 7;
  // There are additional formulas to handle unknown signs
  constexpr static int k_indexOfFirstUnknownSignFormula = 13;

  const Tree* m_angle;
  const Tree* m_cos;
  const Tree* m_sin;
};

}  // namespace Poincare::Internal

#endif
