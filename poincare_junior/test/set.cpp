#include <poincare_junior/src/expression/k_creator.h>
#include <poincare_junior/src/expression/set.h>

#include "helper.h"

using namespace PoincareJ;

QUIZ_CASE(pcj_set) {
  // Set::Add
  Node* set0 = EditionPool::sharedEditionPool()->push<BlockType::Set>(0);
  Set::Add(set0, 1_e);
  Set::Add(set0, 2_e);
  Set::Add(set0, 3_e);
  assert_trees_are_equal(set0, KSet(1_e, 2_e, 3_e));

  // Inclusion
  assert(Set::Includes(set0, 1_e));
  assert(!Set::Includes(set0, 0_e));

  // Pop
  assert_trees_are_equal(Set::Pop(set0), 1_e);

  const Node* set1 = KSet(-1_e, 2_e, 5_e, 6_e, 7_e);
  // Union {2, 3} U {-1, 2, 5, 6, 7}
  EditionReference unionSet = Set::Union(set0, set1);
  assert_trees_are_equal(unionSet, KSet(-1_e, 2_e, 3_e, 5_e, 6_e, 7_e));

  // Intersection {2, 3, 5, 6, 7} ∩ {3, 7, 8_e}
  set0->cloneTreeOverTree(KSet(2_e, 3_e, 5_e, 6_e, 7_e));
  set1 = KSet(3_e, 7_e, 8_e);
  EditionReference intersectionSet = Set::Intersection(set0, set1);
  assert_trees_are_equal(intersectionSet, KSet(3_e, 7_e));

  // Difference {3, 5, 6} \ {2, 5, 6}
  set0->cloneTreeOverTree(KSet(3_e, 5_e, 6_e));
  set1 = KSet(2_e, 5_e, 6_e);
  EditionReference differenceSet = Set::Difference(set0, set1);
  assert_trees_are_equal(differenceSet, KSet(3_e));
}
