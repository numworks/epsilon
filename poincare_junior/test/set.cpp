#include "print.h"
#include <poincare_junior/src/expression/set.h>
#include <poincare_junior/src/expression/simplification.h>
#include <poincare_junior/src/memory/tree_constructor.h>

using namespace Poincare;

void testSet() {
  CachePool * cachePool = CachePool::sharedCachePool();
  EditionPool * editionPool = cachePool->editionPool();

  // Set::Add
  Node set0 = Node::Push<BlockType::Set>(0);
  Set::Add(set0, 1_sn);
  Set::Add(set0, 2_sn);
  Set::Add(set0, 3_n);
  assert(Simplification::Compare(set0, Set(1_sn, 2_sn, 3_n)) == 0);

  print();

  // Inclusion
  assert(Set::Includes(set0, 1_sn));
  assert(!Set::Includes(set0, 0_sn));

  Node set1 = editionPool->initFromTree(Set(1_nsn, 1_sn, 2_sn, 3_n, 5_n, 6_n, 7_n));
  // Union {1, 2, 3} U {-1, 3, 5, 6, 7}
  EditionReference unionSet = Set::Union(set0, set1);
  assert(Simplification::Compare(unionSet.node(), Set(1_nsn, 1_sn, 2_sn, 3_n, 5_n, 6_n, 7_n)) == 0);
  print();

  // Intersection {2, 3, 5, 6, 7} ∩ {3, 7, 8_n}
  set0 = editionPool->initFromTree(Set(2_sn, 3_n, 5_n, 6_n, 7_n));
  set1 = editionPool->initFromTree(Set(3_n, 7_n, 8_n));
  EditionReference intersectionSet = Set::Intersection(set0, set1);
  assert(Simplification::Compare(intersectionSet.node(), Set(3_n, 7_n)) == 0);
  print();

  // Difference {3, 5, 6} \ {2, 5, 6}
  set0 = editionPool->initFromTree(Set(3_n, 5_n, 6_n));
  set1 = editionPool->initFromTree(Set(2_sn, 5_n, 6_n));
  EditionReference differenceSet = Set::Difference(set0, set1);
  assert(Simplification::Compare(differenceSet.node(), Set(3_n)) == 0);
  print();
}
