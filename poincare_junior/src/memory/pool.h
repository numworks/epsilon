#ifndef POINCARE_MEMORY_POOL_H
#define POINCARE_MEMORY_POOL_H

#include <string.h>
#include "type_block.h"
#include "node.h"
#if POINCARE_MEMORY_TREE_LOG
#include <iostream>
#endif

namespace PoincareJ {

class Pool {
  friend class PatternMatching;
public:
  /* We delete the assignment operator because copying without care the
   * ReferenceTable would corrupt the m_referenceTable.m_pool pointer. */
  Pool & operator=(Pool &&) = delete;
  Pool & operator=(const Pool&) = delete;

  Block * blockAtIndex(int i) { return firstBlock() + i; }

  bool contains(Block * block) { return block >= firstBlock() && block < lastBlock(); }
  virtual TypeBlock * firstBlock() = 0;
  virtual Block * lastBlock() = 0;
  size_t size() { return firstBlock() ? lastBlock() - static_cast<Block *>(firstBlock()) : 0; }
  size_t numberOfTrees();

#if POINCARE_MEMORY_TREE_LOG
  enum class LogFormat {
    Flat,
    Tree
  };
#endif
protected:
  class ReferenceTable {
  public:
    constexpr static uint16_t NoNodeIdentifier = 0xFFFF;
    constexpr static uint16_t NumberOfSpecialIdentifier = 1;
    ReferenceTable(Pool * pool) : m_length(0), m_pool(pool) {}
    bool isFull() { return numberOfStoredNodes() == maxNumberOfReferences(); }
    bool isEmpty() const { return numberOfStoredNodes() == 0; }
    size_t numberOfStoredNodes() const { return m_length; }
    virtual uint16_t storeNode(Node node) = 0;
    virtual Node nodeForIdentifier(uint16_t id) const;
    virtual bool reset();
#if POINCARE_MEMORY_TREE_LOG
    void log(std::ostream & stream, LogFormat format, bool verbose = true) const;
    virtual uint16_t identifierForIndex(uint16_t index) const { return index; }
#endif
  protected:
    uint16_t storeNodeAtIndex(Node node, size_t index);
    virtual size_t maxNumberOfReferences() = 0;
    virtual uint16_t * nodeOffsetArray() = 0;
    uint16_t m_length;
    Pool * m_pool;
  };

#if POINCARE_MEMORY_TREE_LOG
public:

  virtual const char * name() = 0;
  void log(std::ostream & stream, LogFormat format, bool verbose);
  virtual const ReferenceTable * referenceTable() const = 0;
  void logReferences(std::ostream & stream, LogFormat format, bool verbose = true) { return referenceTable()->log(stream, format, verbose); }
  __attribute__((__used__)) void log() { log(std::cout, LogFormat::Tree, false); }
  __attribute__((__used__)) void logReferences() { logReferences(std::cout, LogFormat::Tree, false); }
#endif

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
};

}

#endif

