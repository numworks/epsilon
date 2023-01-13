#include "print.h"
#include <poincare_junior/src/expression/set.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace PoincareJ;

void testSet() {
  // Set::Add
  Node set0 = EditionReference::Push<BlockType::Set>(0);
  Set::Add(set0, "1"_n);
  Set::Add(set0, "2"_n);
  Set::Add(set0, "3"_n);
  assert_trees_are_equal(set0, Set("1"_n, "2"_n, "3"_n));

  // Inclusion
  assert(Set::Includes(set0, "1"_n));
  assert(!Set::Includes(set0, "0"_n));

  // Pop
  assert_trees_are_equal(Set::Pop(set0), "1"_n);

  Node set1 = Set("-1"_n, "2"_n, "5"_n, "6"_n, "7"_n);
  // Union {2, 3} U {-1, 2, 5, 6, 7}
  EditionReference unionSet = Set::Union(set0, set1);
  assert_trees_are_equal(unionSet, Set("-1"_n, "2"_n, "3"_n, "5"_n, "6"_n, "7"_n));

  // Intersection {2, 3, 5, 6, 7} ∩ {3, 7, "8"_n}
  set0 = Set("2"_n, "3"_n, "5"_n, "6"_n, "7"_n);
  set1 = Set("3"_n, "7"_n, "8"_n);
  EditionReference intersectionSet = Set::Intersection(set0, set1);
  assert_trees_are_equal(intersectionSet, Set("3"_n, "7"_n));

  // Difference {3, 5, 6} \ {2, 5, 6}
  set0 = Set("3"_n, "5"_n, "6"_n);
  set1 = Set("2"_n, "5"_n, "6"_n);
  EditionReference differenceSet = Set::Difference(set0, set1);
  assert_trees_are_equal(differenceSet, Set("3"_n));
}
QUIZ_CASE(pcj_set) { testSet(); }
