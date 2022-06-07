#ifndef POINCARE_TREE_CACHE_H
#define POINCARE_TREE_CACHE_H

#include "tree_block.h"
#include "tree_sandbox.h"

namespace Poincare {

class TreeCache final : public TreePool {
public:
  static TreeCache * sharedCache();

  TreeBlock * treeForIdentifier(int id);
  int storeLastTree();
  bool copyTreeForEditing(int id);

  TreeBlock * sandboxBlockAtIndex(int i) { return m_sandbox.blockAtIndex(i); }
  void replaceBlock(TreeBlock * previousBlock, TreeBlock newBlock) { return m_sandbox.replaceBlock(previousBlock, newBlock); }
  bool pushBlock(TreeBlock block);
  bool popBlock() { return m_sandbox.popBlock(); }
  TreeSandbox * sandbox() { return &m_sandbox; }

private:
  constexpr static int k_maxNumberOfBlocks = 512;
  constexpr static int k_maxNumberOfCachedTrees = 32;

  TreeCache();
  TreeBlock * firstBlock() override { return m_nextIdentifier == 0 ? nullptr : &m_pool[0]; }
  TreeBlock * lastBlock() override { return m_nextIdentifier == 0 ? &m_pool[0] : nextTree(m_cachedTree[m_nextIdentifier - 1]); }
  void resetCache();

  TreeSandbox m_sandbox;
  TreeBlock m_pool[k_maxNumberOfBlocks];
  int m_nextIdentifier;
  TreeBlock * m_cachedTree[k_maxNumberOfCachedTrees];
};

}

#endif

