#ifndef POINCARE_TREE_SANDBOX_H
#define POINCARE_TREE_SANDBOX_H

#include "tree_block.h"
#include "tree_pool.h"
#include <stddef.h>

namespace Poincare {

class TreeSandbox final : public TreePool {
public:
  TreeSandbox(TypeTreeBlock * firstBlock, size_t size, int numberOfBlocks = 0) :
    m_firstBlock(firstBlock),
    m_numberOfBlocks(numberOfBlocks),
    m_size(size)
  {}

  static TreeSandbox * sharedSandbox();

  void reset() { m_numberOfBlocks = 0; }

  TreeBlock * pushBlock(TreeBlock block);
  void popBlock();
  void replaceBlock(TreeBlock * previousBlock, TreeBlock newBlock);
  bool pushTree(TypeTreeBlock * block);
  void popTree();
  void replaceTree(TypeTreeBlock * previousBlock, TypeTreeBlock * newBlock);
  void moveTree(TreeBlock * destination, TypeTreeBlock * source, size_t * sourceSize = nullptr);
  void removeBlocks(TreeBlock * address, size_t numberOfTreeBlocks);

  TypeTreeBlock * copyTreeFromAddress(const void * address);

  TypeTreeBlock * firstBlock() override { return m_firstBlock; }
  TreeBlock * lastBlock() override { return m_firstBlock + m_numberOfBlocks; }
  TreeBlock * blockAtIndex(int i) { return m_firstBlock + i * sizeof(TreeBlock); }
  size_t size() const { return m_size; }
  void setNumberOfBlocks(int numberOfBlocks) { m_numberOfBlocks = numberOfBlocks; }
  int numberOfBlocks() const { return m_numberOfBlocks; }

private:
  // Pool memory
  bool checkForEnoughSpace(size_t numberOfRequiredBlock);
  void moveBlocks(TreeBlock * destination, TreeBlock * source, size_t numberOfTreeBlocks);
  void freePoolFromNode(TreeBlock * firstBlockToDiscard);

  TypeTreeBlock * m_firstBlock;
  int m_numberOfBlocks;
  size_t m_size;
};

}

#endif
