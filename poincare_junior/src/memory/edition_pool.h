#ifndef POINCARE_MEMORY_EDITION_POOL_H
#define POINCARE_MEMORY_EDITION_POOL_H

#include <omg/global_box.h>
#include <string.h>

#include "tree.h"
#include "type_block.h"
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {
class JuniorLayout;
}

namespace PoincareJ {

typedef void (*ActionWithContext)(void *context, const void *data);

class EditionPool {
  friend class EditionReference;

 public:
  static OMG::GlobalBox<EditionPool> SharedEditionPool;

  EditionPool() : m_referenceTable(this), m_size(0) {}

  uint16_t referenceNode(Tree *node);
  void flush();
  void flushFromBlock(const Block *node);
  void resetRefs() { m_referenceTable.reset(); }
  void deleteIdentifier(uint16_t id) { m_referenceTable.deleteIdentifier(id); }
  void updateIdentifier(uint16_t id, Tree *newNode) {
    m_referenceTable.updateIdentifier(id, newNode);
  }

  typedef bool (*Relax)(void *context);
  constexpr static Relax k_defaultRelax = [](void *context) { return false; };
  void executeAndDump(ActionWithContext action, void *context, const void *data,
                      void *address, int maxSize, Relax relax = k_defaultRelax);
  void executeAndStoreLayout(ActionWithContext action, void *context,
                             const void *data, Poincare::JuniorLayout *layout,
                             Relax relax = k_defaultRelax);
  void executeAndReplaceTree(ActionWithContext action, void *context,
                             Tree *data, Relax relax = k_defaultRelax);

  void replaceBlock(Block *previousBlock, Block newBlock);
  void replaceBlocks(Block *destination, const Block *newBlocks,
                     size_t numberOfBlocks);
  Tree *push(Block block) {
    insertBlock(lastBlock(), block, true);
    return Tree::FromBlocks(lastBlock() - 1);
  }
  bool insertBlock(Block *destination, Block block, bool at = false) {
    return insertBlocks(destination, &block, 1, at);
  }
  bool insertBlocks(Block *destination, const Block *source,
                    size_t numberOfBlocks, bool at = false);
  void popBlock() { removeBlocks(lastBlock() - 1, 1); }
  void removeBlocks(Block *address, size_t numberOfBlocks);
  void moveBlocks(Block *destination, Block *source, size_t numberOfBlocks,
                  bool at = false);

  // Initialize trees
  Tree *initFromAddress(const void *address, bool isTree = true);
  Tree *clone(const Tree *node, bool isTree = true) {
    return initFromAddress(static_cast<const void *>(node->block()), isTree);
  }
  template <BlockType blockType, typename... Types>
  Tree *push(Types... args);

  /* We delete the assignment operator because copying without care the
   * ReferenceTable would corrupt the m_referenceTable.m_pool pointer. */
  EditionPool &operator=(EditionPool &&) = delete;
  EditionPool &operator=(const EditionPool &) = delete;

  Block *blockAtIndex(int i) { return firstBlock() + i; }

  Tree *nodeForIdentifier(uint16_t id) {
    return referenceTable()->nodeForIdentifier(id);
  }
  bool contains(const Block *block) const {
    return block >= firstBlock() && block < lastBlock();
  }
  const Block *firstBlock() const { return m_blocks; }
  Block *firstBlock() { return m_blocks; }
  // If EditionPool is empty, first and last blocks are the same one
  const Block *lastBlock() const { return m_blocks + m_size; }
  Block *lastBlock() { return m_blocks + m_size; }
  size_t size() const { return m_size; }
  size_t numberOfTrees() const;
  // Return true if block is the root node of one of the pool's trees.
  bool isRootBlock(const Block *block, bool allowLast = false) const;

  // Offset in the ReferenceTable are relative to the referenceBlock
  Block *referenceBlock() { return firstBlock(); };

#if POINCARE_TREE_LOG
  enum class LogFormat { Flat, Tree };
#endif
 private:
#if POINCARE_TREE_LOG
 public:
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

 public:
  void setNumberOfBlocks(int numberOfBlocks) { m_size = numberOfBlocks; }

  // Will changing the modified tree alter the other tree ?
  bool isAfter(const Tree *other, Tree *modified) {
    return !contains(other->block()) || other < modified;
  }

  // TODO: not all pool sizes are passing the tests, investigate why
  constexpr static int k_maxNumberOfBlocks = 1024 * 16;
  constexpr static int k_maxNumberOfReferences = k_maxNumberOfBlocks / 8;

 private:
  void execute(ActionWithContext action, void *context, const void *data,
               int maxSize, Relax relax = k_defaultRelax);
  // Pool memory
  void checkForEnoughSpace(size_t numberOfRequiredBlock);
#if POINCARE_TREE_LOG
  const char *name() { return "Edition"; }
#endif
  class ReferenceTable {
    /* The edition pool reference table stores the offset of the tree in the
     * edition pool.
     * - We assume (and assert) that we never referenced more then
     *   k_maxNumberOfEditionReferences at the same time. We make sure of if by
     *  regularly flushing the reference table.
     * - The order of identifiers gives no guarantee on the order of the trees
     *   in the pool.
     */
   public:
    /* Special m_identifier when the reference does not point to a Tree yet. */
    constexpr static uint16_t NoNodeIdentifier = 0xFFFF;
    constexpr static uint16_t NumberOfSpecialIdentifier = 1;
    /* Special offset in the nodeOffsetArray when the pointed Tree has been
     * removed or replaced. */
    constexpr static uint16_t InvalidatedOffset = 0xFFFF;

    ReferenceTable(EditionPool *pool) : m_length(0), m_pool(pool) {}
    Tree *nodeForIdentifier(uint16_t id) const;
    uint16_t storeNode(Tree *node);
    void updateIdentifier(uint16_t id, Tree *newNode);
    void deleteIdentifier(uint16_t id);
    typedef void (*AlterSelectedBlock)(uint16_t *, Block *, const Block *,
                                       const Block *, int);
    void updateNodes(AlterSelectedBlock function,
                     const Block *contextSelection1,
                     const Block *contextSelection2, int contextAlteration);
    void deleteIdentifiersAfterBlock(const Block *block);

    bool isFull() { return numberOfStoredNodes() == maxNumberOfReferences(); }
    bool isEmpty() const { return numberOfStoredNodes() == 0; }
    size_t numberOfStoredNodes() const { return m_length; }
    bool reset();
#if POINCARE_TREE_LOG
    void logIdsForNode(std::ostream &stream, const Tree *node) const;
    uint16_t identifierForIndex(uint16_t index) const { return index; }
#endif
   private:
    /* Special offset in the nodeOffsetArray when the EditionReference that
     * owned it has been deleted. */
    constexpr static uint16_t DeletedOffset = 0xFFFE;

    size_t maxNumberOfReferences() const {
      return EditionPool::k_maxNumberOfReferences;
    }
    uint16_t *nodeOffsetArray() { return m_nodeOffsetForIdentifier; }
    uint16_t storeNodeAtIndex(Tree *node, size_t index);

    uint16_t m_length;
    EditionPool *m_pool;
    uint16_t m_nodeOffsetForIdentifier[EditionPool::k_maxNumberOfReferences];
  };
  const ReferenceTable *referenceTable() const { return &m_referenceTable; }

  /* TODO: if we end up needing too many EditionReference, we could ref-count
   * them in m_referenceTable and implement a destructor on EditionReference. */
  ReferenceTable m_referenceTable;
  Block m_blocks[k_maxNumberOfBlocks];
  size_t m_size;
};

#define SharedEditionPool EditionPool::SharedEditionPool

}  // namespace PoincareJ

#endif
