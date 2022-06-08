#include "tree_cache.h"
#include <assert.h>
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
  assert(m_nextIdentifier < k_maxNumberOfCachedTrees);
  TreeBlock * block = lastBlock();
  m_cachedTree[m_nextIdentifier++] = block;
  size_t numberOfCachedBlocks = lastBlock() - firstBlock();
  m_sandbox = TreeSandbox(lastBlock(), k_maxNumberOfBlocks - numberOfCachedBlocks);
  return m_nextIdentifier - 1;
}

TreeCache::Error TreeCache::copyTreeForEditing(int id) {
  if (m_nextIdentifier <= id) {
    return Error::UninitializedIdentifier;
  }
  size_t treeSize = m_cachedTree[id]->nextTree() - m_cachedTree[id];
  TreeBlock * copiedTree = m_cachedTree[id];
  if (m_sandbox.size() < treeSize) {
    bool reset = resetCache();
    assert(reset); // the tree was at least already cached
  }
  memmove(lastBlock(), copiedTree, treeSize * sizeof(TreeBlock));
  m_sandbox.setNumberOfBlocks(treeSize);
  return Error::None;
}

bool TreeCache::pushBlock(TreeBlock block) {
  if (!m_sandbox.pushBlock(block)) {
    if (!resetCache()) {
      return false;
    }
    // Reinitialize the sandbox with the previous content
    int nbOfSanboxBlocks = m_sandbox.lastBlock() - m_sandbox.firstBlock();
    memmove(m_pool, m_sandbox.firstBlock(), nbOfSanboxBlocks * sizeof(TreeBlock));
    m_sandbox = TreeSandbox(lastBlock(), k_maxNumberOfBlocks);
    m_sandbox.setNumberOfBlocks(nbOfSanboxBlocks);

    // Push new block
    m_sandbox.pushBlock(block);
  }
  return true;
}

TreeCache::TreeCache() :
  m_sandbox(&m_pool[0], k_maxNumberOfBlocks),
  m_nextIdentifier(0)
{
}

bool TreeCache::resetCache() {
  if (m_nextIdentifier == 0) {
    // The cache has already been emptied
    // TODO: trigger an exception checkpoint?
    return false;
  }
  m_nextIdentifier = 0;
  // Redefine sandbox without overriding its content since we might need it
  m_sandbox = TreeSandbox(lastBlock(), k_maxNumberOfBlocks);
  return true;
}

}
