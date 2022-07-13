#ifndef POINCARE_TREE_CACHE_H
#define POINCARE_TREE_CACHE_H

#include "tree_block.h"
#include "tree_sandbox.h"

namespace Poincare {

/*
 * Ref-counter or LRU ?
 * */

class TreeCache final : public TreePool {
public:
  enum class Error {
    None,
    UninitializedIdentifier,
    TreeIsTooBigForSandbox,
  };

  static TreeCache * sharedCache();

  TypeTreeBlock * treeForIdentifier(int id);
  int storeLastTree();
  Error copyTreeForEditing(int id);

  TreeSandbox * sandbox() { return &m_sandbox; }
  bool resetCache(bool preserveSandbox);

private:
  constexpr static int k_maxNumberOfBlocks = 512;
  constexpr static int k_maxNumberOfCachedTrees = 32;

  TreeCache();
  TypeTreeBlock * firstBlock() override { return m_nextIdentifier == 0 ? nullptr : static_cast<TypeTreeBlock *>(&m_pool[0]); }
  TypeTreeBlock * lastBlock() override { return m_nextIdentifier == 0 ? static_cast<TypeTreeBlock *>(&m_pool[0]) : m_cachedTree[m_nextIdentifier - 1]->nextSibling(); }

  TreeSandbox m_sandbox;
  TreeBlock m_pool[k_maxNumberOfBlocks];
  int m_nextIdentifier;
  TypeTreeBlock * m_cachedTree[k_maxNumberOfCachedTrees];
};

}

#endif

