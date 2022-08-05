#include <assert.h>
#include "exception_checkpoint.h"
#include <string.h>
#include "tree_cache.h"

namespace Poincare {

TreeCache * TreeCache::sharedCache() {
  static TreeCache s_cache;
  return &s_cache;
}

TypeTreeBlock * TreeCache::treeForIdentifier(int id) {
  if (id < 0 || id >= m_nextIdentifier) {
    return nullptr;
  }
  return m_cachedTree[id];
}

int TreeCache::storeLastTree() {
  assert(m_nextIdentifier < k_maxNumberOfCachedTrees);
  if (m_sandbox.numberOfBlocks() == 0) {
    return -1;
  }
  if (m_nextIdentifier >= k_maxNumberOfBlocks) {
    reset(true);
  }
  TypeTreeBlock * block = lastBlock();
  m_cachedTree[m_nextIdentifier++] = block;
  size_t numberOfCachedBlocks = lastBlock() - firstBlock();
  m_sandbox = TreeSandbox(lastBlock(), k_maxNumberOfBlocks - numberOfCachedBlocks);
  return m_nextIdentifier - 1;
}

bool TreeCache::reset(bool preserveSandbox) {
  if (m_nextIdentifier == 0) {
    // The cache has already been emptied
    // TODO: trigger an exception checkpoint?
    return false;
  }
  m_nextIdentifier = 0;
  int nbOfSanboxBlocks = preserveSandbox ? m_sandbox.lastBlock() - static_cast<TreeBlock *>(m_sandbox.firstBlock()) : 0;
  if (preserveSandbox) {
    memmove(m_pool, m_sandbox.firstBlock(), nbOfSanboxBlocks * sizeof(TreeBlock));
  }
  // Redefine sandbox without overriding its content since we might need it
  m_sandbox = TreeSandbox(lastBlock(), k_maxNumberOfBlocks, nbOfSanboxBlocks);
  return true;
}

int TreeCache::execute(ActionWithContext action, void * subAction, void * data) {
  ExceptionCheckpoint checkpoint;
start_execute:
  if (ExceptionRun(checkpoint)) {
    bool success = action(subAction, data);
    if (!success) {
      return -1;
    }
    return storeLastTree();
  } else {
    // TODO: don't delete last called treeForIdentifier otherwise can't copyTreeFromAddress if in cache...
    if (!reset(true)) {
      return -1;
    }
    goto start_execute;
  }
}

TreeCache::TreeCache() :
  m_sandbox(static_cast<TypeTreeBlock *>(&m_pool[0]), k_maxNumberOfBlocks),
  m_nextIdentifier(0)
{
}

}
