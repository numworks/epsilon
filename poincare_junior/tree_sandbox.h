#ifndef POINCARE_TREE_SANDBOX_H
#define POINCARE_TREE_SANDBOX_H

#include "tree_block.h"
#include "tree_pool.h"
#include <stddef.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Which API do we want?
 * void insertTreeBefore(Tree * block)
 * void pushCopyOfTree(TreeBlock * block)
 * */

class TreeSandbox final : public TreePool {
public:
  TreeSandbox(TypeTreeBlock * firstBlock, size_t size, int numberOfBlocks = 0) :
    m_firstBlock(firstBlock),
    m_numberOfBlocks(numberOfBlocks),
    m_size(size)
  {}

  bool pushBlock(TreeBlock block);
  void popBlock();
  void replaceBlock(TreeBlock * previousBlock, TreeBlock newBlock);
  bool pushTree(TypeTreeBlock * block);
  void popTree();
  void replaceTree(TypeTreeBlock * previousBlock, TypeTreeBlock * newBlock);
  void moveTree(TreeBlock * destination, TypeTreeBlock * source);

  TypeTreeBlock * copyTreeFromAddress(const void * address, size_t size);

#if POINCARE_TREE_LOG
  void flatLog(std::ostream & stream);
  void treeLog(std::ostream & stream, bool verbose = true);
  __attribute__((__used__)) void log() { treeLog(std::cout, false); }
  __attribute__((__used__)) void verboseLog() { treeLog(std::cout, true); }
#endif

  TypeTreeBlock * firstBlock() override { return m_firstBlock; }
  TreeBlock * lastBlock() override { return m_firstBlock + m_numberOfBlocks; }
  TreeBlock * blockAtIndex(int i) { return m_firstBlock + i * sizeof(TreeBlock); }
  size_t size() const { return m_size; }
  void setNumberOfBlocks(int numberOfBlocks) { m_numberOfBlocks = numberOfBlocks; }
  int numberOfBlocks() const { return m_numberOfBlocks; }

private:
  class Nodes final {
  public:
    Nodes(TypeTreeBlock * block, int numberOfBlocks) : m_block(block), m_numberOfBlocks(numberOfBlocks) {}
    class Iterator : public TypeTreeBlock::Iterator<TypeTreeBlock> {
    public:
      using TypeTreeBlock::Iterator<TypeTreeBlock>::Iterator;
      Iterator & operator++() {
        m_block = m_block->nextNode();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_block); }
    Iterator end() const { return Iterator(m_block + m_numberOfBlocks); }
  private:
    TypeTreeBlock * m_block;
    int m_numberOfBlocks;
  };
  Nodes allNodes() { return Nodes(m_firstBlock, m_numberOfBlocks); }

  class Trees final {
  public:
    Trees(TypeTreeBlock * block, int numberOfBlocks) : m_block(block), m_numberOfBlocks(numberOfBlocks) {}
    class Iterator : public TypeTreeBlock::Iterator<TypeTreeBlock> {
    public:
      using TypeTreeBlock::Iterator<TypeTreeBlock>::Iterator;
      Iterator & operator++() {
        m_block = m_block->nextSibling();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_block); }
    Iterator end() const { return Iterator(m_block + m_numberOfBlocks); }
  private:
    TypeTreeBlock * m_block;
    int m_numberOfBlocks;
  };
  Trees trees() { return Trees(m_firstBlock, m_numberOfBlocks); }

  // Pool memory
  bool checkForEnoughSpace(size_t numberOfRequiredBlock);
  void moveBlocks(TreeBlock * destination, TreeBlock * source, size_t numberOfTreeBlocks);
  void removeBlocks(TreeBlock * address, size_t numberOfTreeBlocks);
  void freePoolFromNode(TreeBlock * firstBlockToDiscard);

  TypeTreeBlock * m_firstBlock;
  int m_numberOfBlocks;
  size_t m_size;
};

}

#endif
