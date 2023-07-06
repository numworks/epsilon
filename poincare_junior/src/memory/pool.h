#ifndef POINCARE_MEMORY_POOL_H
#define POINCARE_MEMORY_POOL_H

#include <string.h>

#include "tree.h"
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

  Tree *nodeForIdentifier(uint16_t id) {
    return referenceTable()->nodeForIdentifier(id);
  }
  bool contains(const Block *block) const {
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
    virtual uint16_t storeNode(Tree *node) = 0;
    virtual Tree *nodeForIdentifier(uint16_t id) const;
    virtual bool reset();
#if POINCARE_MEMORY_TREE_LOG
    void logIdsForNode(std::ostream &stream, const Tree *node) const;
    virtual uint16_t identifierForIndex(uint16_t index) const { return index; }
#endif
   protected:
    uint16_t storeNodeAtIndex(Tree *node, size_t index);
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
  void logNode(std::ostream &stream, const Tree *node, bool recursive,
               bool verbose, int indentation);
  void log(std::ostream &stream, LogFormat format, bool verbose,
           int indentation = 0);
  __attribute__((__used__)) void log() {
    log(std::cout, LogFormat::Tree, false);
  }

#endif

 public:
  Tree::ConstNodeRange allNodes() {
    return Tree::ConstNodeRange(Tree::FromBlocks(firstBlock()),
                                Tree::FromBlocks(lastBlock()));
  }

  Tree::ConstTreeRange trees() {
    return Tree::ConstTreeRange(Tree::FromBlocks(firstBlock()),
                                Tree::FromBlocks(lastBlock()));
  }
};

}  // namespace PoincareJ

#endif
