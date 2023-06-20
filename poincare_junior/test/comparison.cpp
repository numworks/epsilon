#include <poincare_junior/src/expression/k_creator.h>

#include "helper.h"

QUIZ_CASE(pcj_compare) {
  QUIZ_ASSERT(Comparison::Compare("a"_e, "a"_e) == 0);
  QUIZ_ASSERT(Comparison::Compare("a"_e, "b"_e) == -1);
  QUIZ_ASSERT(Comparison::Compare("b"_e, "a"_e) == 1);
  EditionReference c(KAdd(2_e, "a"_e));
  EditionReference d(KAdd(3_e, "a"_e));
  EditionReference e(KAdd(2_e, "b"_e));
  QUIZ_ASSERT(Comparison::Compare(2_e, 3_e) == -1);
  QUIZ_ASSERT(Comparison::Compare(c, d) == -1);
}
