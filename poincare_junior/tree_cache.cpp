#include "tree_cache.h"
#include <string.h>

namespace Poincare {

TreeCache * TreeCache::sharedCache() {
  static TreeCache s_cache;
  return &s_cache;
}

TreeBlock * TreeCache::treeForIdentifier(int id) {
  if (id >= m_nextIdentifier) {
    return nullptr;
  }
  return m_cachedTree[id];
}

int TreeCache::storeLastTree() {
  if (m_nextIdentifier >= k_maxNumberOfCachedTrees) {
    resetCache();
  }
  TreeBlock * block = lastBlock();
  m_cachedTree[m_nextIdentifier++] = block;
  size_t numberOfCachedBlocks = lastBlock() - firstBlock();
  m_sandbox = TreeSandbox(lastBlock(), k_maxNumberOfBlocks - numberOfCachedBlocks);
  return m_nextIdentifier - 1;
}

bool TreeCache::copyTreeForEditing(int id) {
  if (m_nextIdentifier <= id) {
    // TODO error
    return false;
  }
  size_t treeSize = nextTree(m_cachedTree[id]) - m_cachedTree[id];
  memcpy(lastBlock(), m_cachedTree[id], treeSize);
  m_sandbox.setNumberOfBlocks(treeSize);
  return true;
}

bool TreeCache::pushBlock(TreeBlock block) {
  if (!m_sandbox.pushBlock(block)) {
    if (m_nextIdentifier == 0) {
      // The cache has already been emptied
      // TODO: trigger an exception checkpoint?
      return false;
    }
    int nbOfSanboxBlocks = m_sandbox.lastBlock() - m_sandbox.firstBlock();
    resetCache();
    memcpy(m_pool, m_sandbox.firstBlock(), nbOfSanboxBlocks);
    m_sandbox = TreeSandbox(lastBlock(), k_maxNumberOfBlocks);
    m_sandbox.setNumberOfBlocks(nbOfSanboxBlocks);
    m_sandbox.pushBlock(block);
  }
  return true;
}

TreeCache::TreeCache() :
  m_sandbox(&m_pool[0], k_maxNumberOfBlocks),
  m_nextIdentifier(0)
{
}

void TreeCache::resetCache() {
  *this = TreeCache();
}

}
