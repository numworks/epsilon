#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/set.h>

#include "helper.h"

using namespace Poincare::Internal;

QUIZ_CASE(pcj_set) {
  // Set::Add
  Tree* set0 = Set::PushEmpty();
  Set::Add(set0, 1_e);
  Set::Add(set0, 2_e);
  Set::Add(set0, 3_e);
  assert_trees_are_equal(set0, KList(1_e, 2_e, 3_e));

  // Inclusion
  quiz_assert(Set::Includes(set0, 1_e));
  quiz_assert(!Set::Includes(set0, 0_e));

  // Pop
  assert_trees_are_equal(Set::Pop(set0), 1_e);

  // Union {2, 3} U {-1, 2, 5, 6, 7}
  TreeRef unionSet =
      Set::Union(set0, KList(-1_e, 2_e, 5_e, 6_e, 7_e)->cloneTree());
  assert_trees_are_equal(unionSet, KList(-1_e, 2_e, 3_e, 5_e, 6_e, 7_e));

  // Intersection {2, 3, 5, 6, 7} ∩ {3, 7, 8_e}
  set0->cloneTreeOverTree(KList(2_e, 3_e, 5_e, 6_e, 7_e));
  TreeRef intersectionSet =
      Set::Intersection(set0, KList(3_e, 7_e, 8_e)->cloneTree());
  assert_trees_are_equal(intersectionSet, KList(3_e, 7_e));

  // Difference {3, 5, 6} \ {2, 5, 6}
  set0->cloneTreeOverTree(KList(3_e, 5_e, 6_e));
  TreeRef differenceSet =
      Set::Difference(set0, KList(2_e, 5_e, 6_e)->cloneTree());
  assert_trees_are_equal(differenceSet, KList(3_e));
}
