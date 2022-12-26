#include "print.h"
#include <poincare_junior/src/expression/set.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testSet() {
  // Set::Add
  Node set0 = EditionReference::Push<BlockType::Set>(0);
  Set::Add(set0, 1_sn);
  Set::Add(set0, 2_sn);
  Set::Add(set0, 3_n);
  assert_trees_are_equal(set0, Set(1_sn, 2_sn, 3_n));

  // Inclusion
  assert(Set::Includes(set0, 1_sn));
  assert(!Set::Includes(set0, 0_sn));

  // Pop
  assert_trees_are_equal(Set::Pop(set0), 1_sn);

  Node set1 = Set(1_nsn, 2_sn, 5_n, 6_n, 7_n);
  // Union {2, 3} U {-1, 2, 5, 6, 7}
  EditionReference unionSet = Set::Union(set0, set1);
  assert_trees_are_equal(unionSet, Set(1_nsn, 2_sn, 3_n, 5_n, 6_n, 7_n));

  // Intersection {2, 3, 5, 6, 7} ∩ {3, 7, 8_n}
  set0 = Set(2_sn, 3_n, 5_n, 6_n, 7_n);
  set1 = Set(3_n, 7_n, 8_n);
  EditionReference intersectionSet = Set::Intersection(set0, set1);
  assert_trees_are_equal(intersectionSet, Set(3_n, 7_n));

  // Difference {3, 5, 6} \ {2, 5, 6}
  set0 = Set(3_n, 5_n, 6_n);
  set1 = Set(2_sn, 5_n, 6_n);
  EditionReference differenceSet = Set::Difference(set0, set1);
  assert_trees_are_equal(differenceSet, Set(3_n));
}
