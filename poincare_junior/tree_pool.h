#ifndef POINCARE_TREE_POOL_H
#define POINCARE_TREE_POOL_H

#include "tree_block.h"
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

class TreePool {
public:
  TreeBlock * blockAtIndex(int i) { return firstBlock() + sizeof(TreeBlock) * i; }

  virtual TypeTreeBlock * firstBlock() = 0;
  virtual TreeBlock * lastBlock() = 0;

#if POINCARE_TREE_LOG
  void flatLog(std::ostream & stream);
  void treeLog(std::ostream & stream, bool verbose = true);
  __attribute__((__used__)) void log() { treeLog(std::cout, false); }
  __attribute__((__used__)) void verboseLog() { treeLog(std::cout, true); }
#endif

protected:
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
  Nodes allNodes() { return Nodes(firstBlock(), lastBlock() - static_cast<TreeBlock *>(firstBlock())); }

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
  Trees trees() { return Trees(firstBlock(), lastBlock() - static_cast<TreeBlock *>(firstBlock())); }
};

}

#endif

