#ifndef POINCARE_TREE_CACHE_H
#define POINCARE_TREE_CACHE_H

#include "tree_block.h"
#include "tree_sandbox.h"

namespace Poincare {

class TreeCache final : public TreePool {
public:
  static TreeCache * sharedCache();

  const TypeTreeBlock * treeForIdentifier(int id);
  int storeLastTree();

  TreeSandbox * sandbox() { return &m_sandbox; }
  bool reset(bool preserveSandbox);

  typedef bool (*ActionWithContext)(void * subAction, void * data);
  int execute(ActionWithContext action, void * subAction, void * data);

  constexpr static int k_maxNumberOfBlocks = 512;
  constexpr static int k_maxNumberOfCachedTrees = 32;
private:

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

