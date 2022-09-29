#include "poincare_junior/constexpr_node.h"
#include "print.h"

namespace Constants {

constexpr Tree e = u'π'_n;

}

void playWithConstexprNodes() {
  CachePool * cache = CachePool::sharedCachePool();
  EditionPool * editionPool = cache->editionPool();

  constexpr Tree value = 42_n;
  constexpr Tree π = u'π'_n;

  constexpr Tree node = Addition(π, u'e'_n, Constants::e);
  editionPool->initFromTree(node);

  Node valueMod = editionPool->initFromTree(Subtraction(value, 12_n));
  valueMod.expressionInterface()->basicReduction(valueMod.block());

  print();
}
