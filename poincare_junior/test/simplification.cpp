#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/simplification.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_simplification_expansion) {
  EditionReference ref(KPow(e_e, KAdd(1_e, 2_e)));
  ref = Simplification::ExpandReduction(ref);
  assert_trees_are_equal(ref, KMult(KPow(e_e, 1_e), KPow(e_e, 2_e)));
}

QUIZ_CASE(pcj_simplification_contraction) {
  EditionReference ref(KMult(KPow(e_e, 1_e), KPow(e_e, 2_e)));
  ref = Simplification::ContractReduction(ref);
  assert_trees_are_equal(ref, KPow(e_e, KAdd(1_e, 2_e)));
}
