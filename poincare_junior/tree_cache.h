#ifndef POINCARE_TREE_CACHE_H
#define POINCARE_TREE_CACHE_H

#include "tree_block.h"
#include "tree_sandbox.h"

namespace Poincare {

class TreeCache final : public TreePool {
public:
  static TreeCache * sharedCache();

  TreeBlock * treeForIdentifier(int id) { return m_cachedTree[id]; }
  int storeLastTree();

  TreeSandbox sandbox();

private:
  constexpr static int k_numberOfBlocks = 512;
  constexpr static int k_maxNumberOfCachedTrees = 32;

  TreeCache() : m_numberOfCachedTree(0) {}
  TreeBlock * firstBlock() override { return m_numberOfCachedTree == 0 ? 0 : reinterpret_cast<TreeBlock *>(this); }
  TreeBlock * lastBlock() override { return m_numberOfCachedTree == 0 ? reinterpret_cast<TreeBlock *>(this) : nextTree(m_cachedTree[m_numberOfCachedTree - 1]); }


  TreeBlock * m_cachedTree[k_maxNumberOfCachedTrees];
  int m_numberOfCachedTree;
};

}

#endif

