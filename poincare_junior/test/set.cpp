#include "print.h"
#include <poincare_junior/src/expression/set.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

QUIZ_CASE(pcj_set) {
  // Set::Add
  Node set0 = EditionReference::Push<BlockType::Set>(0);
  Set::Add(set0, "1"_e);
  Set::Add(set0, "2"_e);
  Set::Add(set0, "3"_e);
  assert_trees_are_equal(set0, Set("1"_e, "2"_e, "3"_e));

  // Inclusion
  assert(Set::Includes(set0, "1"_e));
  assert(!Set::Includes(set0, "0"_e));

  // Pop
  assert_trees_are_equal(Set::Pop(set0), "1"_e);

  Node set1 = Set("-1"_e, "2"_e, "5"_e, "6"_e, "7"_e);
  // Union {2, 3} U {-1, 2, 5, 6, 7}
  EditionReference unionSet = Set::Union(set0, set1);
  assert_trees_are_equal(unionSet, Set("-1"_e, "2"_e, "3"_e, "5"_e, "6"_e, "7"_e));

  // Intersection {2, 3, 5, 6, 7} ∩ {3, 7, "8"_e}
  set0 = Set("2"_e, "3"_e, "5"_e, "6"_e, "7"_e);
  set1 = Set("3"_e, "7"_e, "8"_e);
  EditionReference intersectionSet = Set::Intersection(set0, set1);
  assert_trees_are_equal(intersectionSet, Set("3"_e, "7"_e));

  // Difference {3, 5, 6} \ {2, 5, 6}
  set0 = Set("3"_e, "5"_e, "6"_e);
  set1 = Set("2"_e, "5"_e, "6"_e);
  EditionReference differenceSet = Set::Difference(set0, set1);
  assert_trees_are_equal(differenceSet, Set("3"_e));
}
