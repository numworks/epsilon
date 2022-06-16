#include "tree_block.h"
#include <assert.h>

namespace Poincare {

const char * TreeBlock::log() {
  BlockType type = static_cast<BlockType>(m_content);
  switch (type) {
    case BlockType::Addition:
      return "Addition";
    case BlockType::Multiplication:
      return "Multiplication";
    case BlockType::Integer:
      return "Integer";
    default:
      static char s_buffer[2] = "0";
      s_buffer[0] = '0'+ m_content;
      return s_buffer;
  }
}

int TreeBlock::numberOfSubtrees() const {
  BlockType type = static_cast<BlockType>(m_content);
  switch (type) {
    case BlockType::Addition:
    case BlockType::Multiplication:
      return 2;
    case BlockType::Integer:
      return 1;
    default:
      return 0;
  }
}

TreeBlock * TreeBlock::nextTree() {
  int nbOfSubtreesToScan = numberOfSubtrees();
  TreeBlock * result = this;
  while (nbOfSubtreesToScan > 0) {
    result = result->nextBlock();
    nbOfSubtreesToScan += result->numberOfSubtrees() - 1;
  }
  return result->nextBlock();
}

TreeBlock::BackwardsDirect::BackwardsDirect(const TreeBlock * block) :
  m_memoizer(const_cast<TreeBlock *>(block))
{}

TreeBlock::BackwardsDirect::Iterator::Memoizer::Memoizer(TreeBlock * treeBlock) :
  m_block(treeBlock),
  m_firstMemoizedSubtreeIndex(0),
  m_firstSubtreeIndex(0),
  m_numberOfSubtrees(treeBlock->numberOfSubtrees())
{
  memoizeUntilIndex(m_numberOfSubtrees);
}

TreeBlock * TreeBlock::BackwardsDirect::Iterator::Memoizer::subtreeAtIndex(int i) {
  if (i < m_firstSubtreeIndex || i >= m_firstSubtreeIndex + m_numberOfSubtrees) {
    memoizeUntilIndex(i + 1);
  }
  assert(i >= m_firstSubtreeIndex && i < m_firstSubtreeIndex + m_numberOfSubtrees);
  return m_subtrees[(m_firstMemoizedSubtreeIndex + i - m_firstSubtreeIndex) % k_maxNumberOfMemoizedSubtrees];
}

void TreeBlock::BackwardsDirect::Iterator::Memoizer::memoizeUntilIndex(int i) {
  int counter = 0;
  for (TreeBlock * block : m_block->directSubtrees()) {
    m_subtrees[counter % k_maxNumberOfMemoizedSubtrees] = block;
    counter++;
    if (counter == i) {
      break;
    }
  }
  if (i < k_maxNumberOfMemoizedSubtrees) {
    m_firstMemoizedSubtreeIndex = 0;
  } else {
    m_firstMemoizedSubtreeIndex = counter % k_maxNumberOfMemoizedSubtrees;
  }
}

}
