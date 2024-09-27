
#include <omg/float.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/expression/trigonometry_exact_formulas.h>

#include "helper.h"

using namespace Poincare::Internal;

/* Using a class made friend of ExactFormula to access private members and
 * methods. */

class Poincare::Internal::ExactFormulaTest {
 public:
  static void testExactFormulas() {
    for (int i = 0; i < ExactFormula::k_totalNumberOfFormula; i++) {
      ExactFormula ef = ExactFormula::GetExactFormulaAtIndex(i);
      // If they can be deep reduced, exact formulas should be updated.
      Tree* a = ef.m_angle->cloneTree();
      quiz_assert(!SystematicReduction::DeepReduce(a));
      a->cloneTreeOverTree(ef.m_cos);
      quiz_assert(!SystematicReduction::DeepReduce(a));
      a->cloneTreeOverTree(ef.m_sin);
      quiz_assert(!SystematicReduction::DeepReduce(a));
      a->removeTree();
      if (i >= ExactFormula::k_indexOfFirstUnknownSignFormula) {
        // If sign of these formulas can be computed, they  must be removed.
        assert(ef.m_cos->isUndef() != ef.m_sin->isUndef());
        Sign sign = ef.m_sin->isUndef() ? GetSign(ef.m_cos) : GetSign(ef.m_sin);
        quiz_assert(sign.canBeStrictlyNegative() &&
                    sign.canBeStrictlyPositive());
      } else {
        assert(!ef.m_cos->isUndef() && !ef.m_sin->isUndef());
      }
      // Check with approximation that exact formulas are correct.
      float angle = Approximation::To<float>(ef.m_angle);
      float epsilon = OMG::Float::EpsilonLax<float>();
      if (!ef.m_cos->isUndef()) {
        float cos = Approximation::To<float>(ef.m_cos);
        quiz_assert(
            OMG::Float::RoughlyEqual<float>(std::cos(angle), cos, epsilon));
        quiz_assert(
            OMG::Float::RoughlyEqual<float>(std::acos(cos), angle, epsilon));
      }
      if (!ef.m_sin->isUndef()) {
        float sin = Approximation::To<float>(ef.m_sin);
        quiz_assert(
            OMG::Float::RoughlyEqual<float>(std::sin(angle), sin, epsilon));
        quiz_assert(
            OMG::Float::RoughlyEqual<float>(std::asin(sin), angle, epsilon));
      }
    }
    /* If pi/2 - asin(x) => acos(x) and pi/2 - acos(x) => asin(x), exact
     * formulas for angles in ]π/4, π/2] must be removed. */
    process_tree_and_compare("π/2-arcsin(1/10)", "π/2-arcsin(1/10)",
                             [](Tree* tree, ProjectionContext ctx) {
                               Simplification::SimplifyWithAdaptiveStrategy(
                                   tree, &ctx);
                             },
                             {});
    process_tree_and_compare("π/2-arccos(1/10)", "π/2-arccos(1/10)",
                             [](Tree* tree, ProjectionContext ctx) {
                               Simplification::SimplifyWithAdaptiveStrategy(
                                   tree, &ctx);
                             },
                             {});
  }
};

QUIZ_CASE(pcj_trigonometry_exact_formulas) {
  ExactFormulaTest::testExactFormulas();
}
