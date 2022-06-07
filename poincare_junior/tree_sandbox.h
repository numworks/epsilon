#ifndef POINCARE_TREE_SANDBOX_H
#define POINCARE_TREE_SANDBOX_H

#include "tree_block.h"
#include "tree_pool.h"
#include <stddef.h>

namespace Poincare {

class TreeSandbox final : public TreePool {
public:
  TreeSandbox(TreeBlock * firstBlock, size_t size) :
    m_firstBlock(firstBlock),
    m_numberOfBlocks(0),
    m_size(size)
  {}

  void replaceBlock(TreeBlock * previousBlock, TreeBlock newBlock);
  bool pushBlock(TreeBlock block);
  bool popBlock();

  TreeBlock * firstBlock() override { return m_firstBlock; }
  TreeBlock * lastBlock() override { return m_firstBlock + m_numberOfBlocks; }
  void setNumberOfBlocks(int numberOfBlocks) { m_numberOfBlocks = numberOfBlocks; }


private:
  TreeBlock * m_firstBlock;
  int m_numberOfBlocks;
  size_t m_size;
};

}

#endif

