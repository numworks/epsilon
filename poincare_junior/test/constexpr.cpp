#include <poincare_junior/src/memory/constexpr_node.h>
#include "print.h"

namespace Constants {

constexpr Tree e = u'π'_n;

}

void playWithConstexprNodes() {
  CachePool * cache = CachePool::sharedCachePool();
  EditionPool * editionPool = cache->editionPool();

  constexpr Tree value = 42_n;
  constexpr Tree π = u'π'_n;

  constexpr Tree node = Add(π, u'e'_n, Constants::e);
  editionPool->initFromTree(node);

  Node valueMod = editionPool->initFromTree(Sub(value, 12_n));
  EExpression::BasicReduction(valueMod.block());

  print();
}
