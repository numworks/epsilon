#include "tree_cache.h"
#include "helpers.h"
#include "tree_sandbox.h"
#include <assert.h>

namespace Poincare {

bool TreeSandbox::execute(TypeTreeBlock * address, TreeEditor action) {
  return privateExecuteAction(action, address);
}

bool TreeSandbox::execute(int treeId, TreeEditor action) {
  return privateExecuteAction(action, nullptr, treeId);
}

TreeBlock * TreeSandbox::pushBlock(TreeBlock block) {
  if (!checkForEnoughSpace(1)) {
    return nullptr;
  }
  assert(m_numberOfBlocks < m_size);
  *lastBlock() = block;
  m_numberOfBlocks++;
  return lastBlock() - 1;
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
  while (treeSize > 0) {
    pushBlock(*block);
    block++;
    treeSize--;
  }
  return true;
}

void TreeSandbox::popTree() {
  size_t lastBlockSize = static_cast<TypeTreeBlock *>(lastBlock() - 1)->treeSize();
  assert(m_numberOfBlocks >= lastBlockSize);
  m_numberOfBlocks -= lastBlockSize;
}

void TreeSandbox::replaceTree(TypeTreeBlock * previousBlock, TypeTreeBlock * newBlock) {
  size_t newTreeSize = newBlock->treeSize();
  size_t previousTreeSize = previousBlock->treeSize();
  // previousBlock can't be a subtree of newBlock
  assert(!(previousBlock >= newBlock && previousBlock < newBlock + newTreeSize));
  if (newBlock >= previousBlock && newBlock < previousBlock + previousTreeSize) {
    // newBlock can be a subtree of previousBlock but previousTreeSize has to be adjusted
    previousTreeSize -= newTreeSize;
  }
  moveBlocks(previousBlock, newBlock, newTreeSize);
  removeBlocks(previousBlock + newTreeSize, previousTreeSize);
}

void TreeSandbox::moveTree(TreeBlock * destination, TypeTreeBlock * source, size_t * treeSize) {
  size_t size = treeSize ? *treeSize : source->treeSize();
  moveBlocks(destination, source, size);
}

void TreeSandbox::removeBlocks(TreeBlock * address, size_t numberOfTreeBlocks) {
  size_t numberOfBlocksAfterRemoved = m_numberOfBlocks - (address - static_cast<TreeBlock *>(m_firstBlock)) - numberOfTreeBlocks;
  moveBlocks(address, address + numberOfTreeBlocks, numberOfBlocksAfterRemoved);
  m_numberOfBlocks -= numberOfTreeBlocks;
}

TypeTreeBlock * TreeSandbox::copyTreeFromAddress(const void * address) {
  size_t size = reinterpret_cast<const TypeTreeBlock *>(address)->treeSize();
  if (!checkForEnoughSpace(size)) {
    return nullptr;
  }
  TypeTreeBlock * copiedTree = static_cast<TypeTreeBlock *>(lastBlock());
  memcpy(copiedTree, address, size * sizeof(TreeBlock));
  m_numberOfBlocks += size;
  return copiedTree;
}

bool TreeSandbox::checkForEnoughSpace(size_t numberOfRequiredBlock) {
  if (m_numberOfBlocks + numberOfRequiredBlock > m_size) {
    // TODO raise sandbox memory full error
    return false;
  }
  return true;
}

void TreeSandbox::moveBlocks(TreeBlock * destination, TreeBlock * source, size_t numberOfTreeBlocks) {
  uint8_t * src = reinterpret_cast<uint8_t *>(source);
  uint8_t * dst = reinterpret_cast<uint8_t *>(destination);
  size_t len = numberOfTreeBlocks * sizeof(TreeBlock);
  Helpers::Rotate(dst, src, len);
}

void TreeSandbox::freePoolFromNode(TreeBlock * firstBlockToDiscard) {
  m_numberOfBlocks = firstBlockToDiscard - static_cast<TreeBlock *>(m_firstBlock);
}

bool TreeSandbox::privateExecuteAction(TreeEditor action, TypeTreeBlock * address, int treeId) {
start_execute:
  if (true) {//if (setCheckpoint()) { // TODO
    if (!address) {
      assert(treeId >= 0);
      address = TreeCache::sharedCache()->treeForIdentifier(treeId);
      if (!address) {
        return false;
      }
    }
    TypeTreeBlock * tree = copyTreeFromAddress(address);
    if (!tree) {
      return false;
    }
    action(tree, this);
    return true;
  } else {
    // TODO: don't delete last called treeForIdentifier otherwise can't copyTreeFromAddress if in cache...
    if (!TreeCache::sharedCache()->reset(true)) {
      return false;
    }
    goto start_execute;
  }
}

}
