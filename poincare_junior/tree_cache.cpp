#include "tree_cache.h"

namespace Poincare {

TreeCache * TreeCache::sharedCache() {
  static TreeCache s_cache;
  return &s_cache;
}

int TreeCache::storeLastTree() {
  TreeBlock * block = lastBlock();
  m_cachedTree[m_numberOfCachedTree++] = block;
  return m_numberOfCachedTree - 1;
}

TreeSandbox TreeCache::sandbox() {
  size_t numberOfCachedBlocks = m_numberOfCachedTree == 0 ? 0 : lastBlock() - firstBlock();
  return TreeSandbox(lastBlock(), k_numberOfBlocks - numberOfCachedBlocks);
}

}
