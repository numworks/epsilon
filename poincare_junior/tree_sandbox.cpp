#include "tree_cache.h"
#include "tree_sandbox.h"
#include <assert.h>

namespace Poincare {

void TreeSandbox::replaceBlock(TreeBlock * previousBlock, TreeBlock newBlock) {
  *previousBlock = newBlock;
}

bool TreeSandbox::pushBlock(TreeBlock block) {
  if (m_numberOfBlocks >= m_size) {
    if (!TreeCache::sharedCache()->resetCache(true)) {
      return false;
    }
  }
  assert(m_numberOfBlocks < m_size);
  *lastBlock() = block;
  m_numberOfBlocks++;
  return true;
}

bool TreeSandbox::popBlock() {
  if (m_numberOfBlocks-- > 0) {
    return true;
  }
  return false;
}

}
