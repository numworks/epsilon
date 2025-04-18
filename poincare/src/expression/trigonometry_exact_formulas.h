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
  template <KTreeConcept T1, KTreeConcept T2, KTreeConcept T3, KTreeConcept T4>
  constexpr ExactFormula(T1 angle, T2 cos, T3 sin, T4 tan)
      : m_angle(angle), m_cos(cos), m_sin(sin), m_tan(tan) {}
  // Find exact formula corresponding to angle, nullptr otherwise
  static const Tree* GetTrigOf(const Tree* angle, Type type);
  // Find exact formula corresponding to trigonometry, nullptr otherwise
  static const Tree* GetAngleOf(const Tree* trig, Type type);

 private:
  static ExactFormula GetExactFormulaAtIndex(int n);

  constexpr static int k_totalNumberOfFormula = 13;
  // Only formulas for angles in [0, π/4] are used when simplifying cos and sin
  constexpr static int k_numberOfFormulaForTrig = 7;

  const Tree* m_angle;
  const Tree* m_cos;
  const Tree* m_sin;
  const Tree* m_tan;
};

}  // namespace Poincare::Internal

#endif
