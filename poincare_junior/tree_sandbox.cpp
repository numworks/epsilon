#include "tree_cache.h"
#include "helpers.h"
#include "tree_sandbox.h"
#include <assert.h>

namespace Poincare {

bool TreeSandbox::pushBlock(TreeBlock block) {
  if (!checkForEnoughSpace(1)) {
    return false;
  }
  assert(m_numberOfBlocks < m_size);
  *lastBlock() = block;
  m_numberOfBlocks++;
  return true;
}

void TreeSandbox::popBlock() {
  assert(m_numberOfBlocks > 0);
  m_numberOfBlocks--;
}

void TreeSandbox::replaceBlock(TreeBlock * previousBlock, TreeBlock newBlock) {
  *previousBlock = newBlock;
}

bool TreeSandbox::pushTree(TypeTreeBlock * block) {
  size_t treeSize = block->treeSize();
  if (!checkForEnoughSpace(treeSize)) {
    return false;
  }
  moveBlocks(m_firstBlock + m_numberOfBlocks, block, block->treeSize());
  return true;
}

void TreeSandbox::popTree() {
  size_t lastBlockSize = static_cast<TypeTreeBlock *>(lastBlock() - 1)->treeSize();
  assert(m_numberOfBlocks >= lastBlockSize);
  m_numberOfBlocks -= lastBlockSize;
}

void TreeSandbox::replaceTree(TypeTreeBlock * previousBlock, TypeTreeBlock * newBlock) {
  size_t newTreeSize = newBlock->treeSize();
  moveBlocks(previousBlock, newBlock, newTreeSize);
  size_t previousTreeSize = previousBlock->treeSize();
  removeBlocks(previousBlock + newTreeSize, previousTreeSize);
}

void TreeSandbox::moveTree(TreeBlock * destination, TypeTreeBlock * source) {
  moveBlocks(destination, source, source->treeSize());
}

TypeTreeBlock * TreeSandbox::copyTreeFromAddress(const void * address, size_t size) {
  size_t sizeOfTreeInBlocks = size/sizeof(TreeBlock);
  if (!checkForEnoughSpace(sizeOfTreeInBlocks)) {
    return nullptr;
  }
  TypeTreeBlock * copiedTree = static_cast<TypeTreeBlock *>(lastBlock());
  memcpy(copiedTree, address, size);
  m_numberOfBlocks += sizeOfTreeInBlocks;
  return copiedTree;
}

bool TreeSandbox::checkForEnoughSpace(size_t numberOfRequiredBlock) {
  if (m_numberOfBlocks + numberOfRequiredBlock > m_size) {
    if (!TreeCache::sharedCache()->resetCache(true)) {
      return false;
    }
    return m_numberOfBlocks + numberOfRequiredBlock <= m_size;
  }
  return true;
}

void TreeSandbox::moveBlocks(TreeBlock * destination, TreeBlock * source, size_t numberOfTreeBlocks) {
  uint32_t * src = reinterpret_cast<uint32_t *>(source);
  uint32_t * dst = reinterpret_cast<uint32_t *>(destination);
  size_t len = numberOfTreeBlocks * sizeof(TreeBlock) / sizeof(uint32_t);
  Helpers::Rotate(dst, src, len);
}

void TreeSandbox::removeBlocks(TreeBlock * address, size_t numberOfTreeBlocks) {
  size_t numberOfBlocksAfterRemoved = m_numberOfBlocks - (address - static_cast<TreeBlock *>(m_firstBlock)) - numberOfTreeBlocks;
  moveBlocks(address, address + numberOfTreeBlocks, numberOfBlocksAfterRemoved);
  m_numberOfBlocks -= numberOfTreeBlocks;
}

void TreeSandbox::freePoolFromNode(TreeBlock * firstBlockToDiscard) {
  m_numberOfBlocks = firstBlockToDiscard - static_cast<TreeBlock *>(m_firstBlock);
}


}
