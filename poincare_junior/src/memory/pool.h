#ifndef POINCARE_MEMORY_POOL_H
#define POINCARE_MEMORY_POOL_H

#include <string.h>

#include "node.h"
#include "type_block.h"
#if POINCARE_MEMORY_TREE_LOG
#include <iostream>
#endif

namespace PoincareJ {

class Pool {
 public:
  /* We delete the assignment operator because copying without care the
   * ReferenceTable would corrupt the m_referenceTable.m_pool pointer. */
  Pool &operator=(Pool &&) = delete;
  Pool &operator=(const Pool &) = delete;

  Block *blockAtIndex(int i) { return firstBlock() + i; }

  const Node *nodeForIdentifier(uint16_t id) {
    return referenceTable()->nodeForIdentifier(id);
  }
  bool contains(Block *block) const {
    return block >= firstBlock() && block < lastBlock();
  }
  virtual const TypeBlock *firstBlock() const = 0;
  TypeBlock *firstBlock() {
    return const_cast<TypeBlock *>(
        const_cast<const Pool *>(this)->firstBlock());
  }
  virtual const TypeBlock *lastBlock() const = 0;
  TypeBlock *lastBlock() {
    return const_cast<TypeBlock *>(const_cast<const Pool *>(this)->lastBlock());
  }
  size_t size() const { return lastBlock() - firstBlock(); }
  size_t numberOfTrees() const;

#if POINCARE_MEMORY_TREE_LOG
  enum class LogFormat { Flat, Tree };
#endif
 protected:
  class ReferenceTable {
   public:
    constexpr static uint16_t NoNodeIdentifier = 0xFFFF;
    constexpr static uint16_t UninitializedOffset = 0xFFFF;
    constexpr static uint16_t NumberOfSpecialIdentifier = 1;
    ReferenceTable(Pool *pool) : m_length(0), m_pool(pool) {}
    bool isFull() { return numberOfStoredNodes() == maxNumberOfReferences(); }
    bool isEmpty() const { return numberOfStoredNodes() == 0; }
    size_t numberOfStoredNodes() const { return m_length; }
    virtual uint16_t storeNode(Node *node) = 0;
    virtual Node *nodeForIdentifier(uint16_t id) const;
    virtual bool reset();
#if POINCARE_MEMORY_TREE_LOG
    void logIdsForNode(std::ostream &stream, Node *node) const;
    virtual uint16_t identifierForIndex(uint16_t index) const { return index; }
#endif
   protected:
    uint16_t storeNodeAtIndex(Node *node, size_t index);
    virtual size_t maxNumberOfReferences() = 0;
    virtual uint16_t *nodeOffsetArray() = 0;
    uint16_t m_length;
    Pool *m_pool;
  };

 private:
  virtual const ReferenceTable *referenceTable() const = 0;

#if POINCARE_MEMORY_TREE_LOG
 public:
  virtual const char *name() = 0;
  void logNode(std::ostream &stream, Node *node, bool recursive, bool verbose,
               int indentation);
  void log(std::ostream &stream, LogFormat format, bool verbose,
           int indentation = 0);
  __attribute__((__used__)) void log() {
    log(std::cout, LogFormat::Tree, false);
  }

#endif

 protected:
  class AbstractIterator {
   public:
    AbstractIterator(const TypeBlock *block)
        : m_node(const_cast<TypeBlock *>(block)) {}
    const Node *operator*() { return m_node; }
    bool operator!=(const AbstractIterator &it) const {
      return (m_node.block() != it.m_node.block());
    }

   protected:
    Node *m_node;
  };

 public:
  class Nodes final {
   public:
    Nodes(TypeBlock *block, int numberOfBlocks)
        : m_node(numberOfBlocks > 0 ? block : nullptr),
          m_numberOfBlocks(numberOfBlocks) {}
    class Iterator : public AbstractIterator {
     public:
      using AbstractIterator::AbstractIterator;
      Iterator &operator++() {
        m_node = m_node.nextNode();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node.block()); }
    Iterator end() const { return Iterator(m_node.block() + m_numberOfBlocks); }

   private:
    Node *m_node;
    int m_numberOfBlocks;
  };
  Nodes allNodes() { return Nodes(firstBlock(), size()); }

  class Trees final {
   public:
    Trees(TypeBlock *block, int numberOfBlocks)
        : m_node(numberOfBlocks > 0 ? block : nullptr),
          m_numberOfBlocks(numberOfBlocks) {}
    class Iterator : public AbstractIterator {
     public:
      using AbstractIterator::AbstractIterator;
      Iterator &operator++() {
        m_node = m_node.nextTree();
        return *this;
      }
    };
    Iterator begin() const { return Iterator(m_node.block()); }
    Iterator end() const { return Iterator(m_node.block() + m_numberOfBlocks); }

   private:
    Node *m_node;
    int m_numberOfBlocks;
  };
  Trees trees() { return Trees(firstBlock(), size()); }
};

}  // namespace PoincareJ

#endif
