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
  virtual const Block *firstBlock() const = 0;
  Block *firstBlock() {
    return const_cast<Block *>(const_cast<const Pool *>(this)->firstBlock());
  }
  virtual const Block *lastBlock() const = 0;
  Block *lastBlock() {
    return const_cast<Block *>(const_cast<const Pool *>(this)->lastBlock());
  }
  size_t size() const { return lastBlock() - firstBlock(); }
  size_t numberOfTrees() const;
  // Return true if block is the root node of one of the pool's trees.
  bool isRootBlock(const Block *block, bool allowLast = false) const;

  // Offset in the ReferenceTable are relative to the referenceBlock
  virtual Block *referenceBlock() { return firstBlock(); };

#if POINCARE_MEMORY_TREE_LOG
  enum class LogFormat { Flat, Tree };
#endif
 protected:
  class ReferenceTable {
   public:
    /* Special m_identifier when the reference does not point to a Tree yet. */
    constexpr static uint16_t NoNodeIdentifier = 0xFFFF;
    constexpr static uint16_t NumberOfSpecialIdentifier = 1;
    /* Special offset in the nodeOffsetArray when the pointed Tree has been
     * removed or replaced. */
    constexpr static uint16_t InvalidatedOffset = 0xFFFF;
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
    virtual size_t maxNumberOfReferences() const = 0;
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
  Tree::ConstNodes allNodes() {
    return Tree::ConstNodes(Tree::FromBlocks(firstBlock()), numberOfTrees());
  }

  Tree::ConstTrees trees() {
    return Tree::ConstTrees(Tree::FromBlocks(firstBlock()), numberOfTrees());
  }
};

}  // namespace PoincareJ

#endif
