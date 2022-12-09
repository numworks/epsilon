#ifndef POINCARE_MEMORY_POOL_H
#define POINCARE_MEMORY_POOL_H

#include "type_block.h"
#include "node.h"
#if POINCARE_MEMORY_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

class Pool {
public:
  /* We delete the assignment operator because copying without care the
   * ReferenceTable would corrupt the m_referenceTable.m_pool pointer. */
  Pool & operator=(Pool &&) = delete;
  Pool & operator=(const Pool&) = delete;

  Block * blockAtIndex(int i) { return firstBlock() + sizeof(Block) * i; }

  virtual TypeBlock * firstBlock() = 0;
  virtual Block * lastBlock() = 0;
  size_t size() { return firstBlock() ? lastBlock() - static_cast<Block *>(firstBlock()) : 0; }
  size_t numberOfTrees();

  constexpr static int k_maxNumberOfReferences = 128;
protected:

  class ReferenceTable {
  public:
    constexpr static uint16_t NoNodeIdentifier = 0xFFFF;
    constexpr static uint16_t NumberOfSpecialIdentifier = 1;
    ReferenceTable(Pool * pool) : m_length(0), m_pool(pool) {}
    bool isFull() { return numberOfStoredNodes() == Pool::k_maxNumberOfReferences; }
    bool isEmpty() const { return numberOfStoredNodes() == 0; }
    int numberOfStoredNodes() const { return m_length; }
    virtual uint16_t storeNode(Node node);
    virtual Node nodeForIdentifier(uint16_t id) const;
    virtual bool reset();
  protected:
    uint16_t m_nodeOffsetForIdentifier[Pool::k_maxNumberOfReferences];
    uint16_t m_length;
    Pool * m_pool;
  };

#if POINCARE_MEMORY_TREE_LOG
public:
  void flatLog(std::ostream & stream);
  void treeLog(std::ostream & stream, bool verbose = true);
  __attribute__((__used__)) void log() { treeLog(std::cout, false); }
  __attribute__((__used__)) void verboseLog() { treeLog(std::cout, true); }

protected:

  class AbstractIterator {
  public:
    AbstractIterator(const TypeBlock * block) : m_node(const_cast<TypeBlock *>(block)) {}
    const Node operator*() { return m_node; }
    bool operator!=(const AbstractIterator& it) const { return (m_node.block() != it.m_node.block()); }
  protected:
    Node m_node;
  };

  class Nodes final {
  public:
    Nodes(TypeBlock * block, int numberOfBlocks) : m_node(block), m_numberOfBlocks(numberOfBlocks) {}
    class Iterator : public AbstractIterator {
    public:
      using AbstractIterator::AbstractIterator;
      Iterator & operator++() {
        m_node = m_node.nextNode();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node.block()); }
    Iterator end() const { return Iterator(m_node.block() + m_numberOfBlocks); }
  private:
    Node m_node;
    int m_numberOfBlocks;
  };
  Nodes allNodes();

  class Trees final {
  public:
    Trees(TypeBlock * block, int numberOfBlocks) : m_node(block), m_numberOfBlocks(numberOfBlocks) {}
    class Iterator : public AbstractIterator {
    public:
      using AbstractIterator::AbstractIterator;
      Iterator & operator++() {
        m_node = m_node.nextTree();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node.block()); }
    Iterator end() const { return Iterator(m_node.block() + m_numberOfBlocks); }
  private:
    Node m_node;
    int m_numberOfBlocks;
  };
  Trees trees();
#endif
};

}

#endif

