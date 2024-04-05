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

class EditionPool {
  friend class TreeRef;

 public:
  // TODO: not all pool sizes are passing the tests, investigate why
  constexpr static int k_maxNumberOfBlocks = 1024 * 16;
  constexpr static int k_maxNumberOfReferences = k_maxNumberOfBlocks / 8;

  static OMG::GlobalBox<EditionPool> SharedEditionPool;

  EditionPool() : m_referenceTable(this), m_size(0) {}

  const Block *firstBlock() const { return m_blocks; }
  Block *firstBlock() { return m_blocks; }
  // If EditionPool is empty, first and last blocks are the same one
  const Block *lastBlock() const { return m_blocks + m_size; }
  Block *lastBlock() { return m_blocks + m_size; }
  size_t size() const { return m_size; }
  size_t numberOfTrees() const;
  // Offset in the ReferenceTable are relative to the referenceBlock
  Block *referenceBlock() { return firstBlock(); }
  Block *blockAtIndex(int i) { return firstBlock() + i; }

  bool contains(const Block *block) const {
    return block >= firstBlock() && block < lastBlock();
  }
  // Return true if block is the root node of one of the pool's trees.
  bool isRootBlock(const Block *block, bool allowLast = false) const;
  // Will changing the modified tree alter the other tree ?
  bool isAfter(const Tree *other, Tree *modified) {
    return !contains(other->block()) || other < modified;
  }

  // Initialize trees
  Tree *initFromAddress(const void *address, bool isTree = true);
  Tree *clone(const Tree *node, bool isTree = true) {
    return initFromAddress(static_cast<const void *>(node->block()), isTree);
  }
  template <Type blockType, typename... Types>
  Tree *push(Types... args);
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

  void flush();
  void flushFromBlock(const Block *node);
  uint16_t referenceNode(Tree *node);
  void deleteIdentifier(uint16_t id) { m_referenceTable.deleteIdentifier(id); }
  void updateIdentifier(uint16_t id, Tree *newNode) {
    m_referenceTable.updateIdentifier(id, newNode);
  }
  Tree *nodeForIdentifier(uint16_t id) {
    return m_referenceTable.nodeForIdentifier(id);
  }

  typedef void (*ActionWithContext)(void *context, const void *data);
  typedef bool (*Relax)(void *context);
  constexpr static Relax k_defaultRelax = [](void *context) { return false; };
  void executeAndStoreLayout(ActionWithContext action, void *context,
                             const void *data, Poincare::JuniorLayout *layout,
                             Relax relax = k_defaultRelax);
  void executeAndReplaceTree(ActionWithContext action, void *context,
                             Tree *data, Relax relax = k_defaultRelax);

  /* We delete the assignment operator because copying without care the
   * ReferenceTable would corrupt the m_referenceTable.m_pool pointer. */
  EditionPool &operator=(EditionPool &&) = delete;
  EditionPool &operator=(const EditionPool &) = delete;

#if POINCARE_TREE_LOG
  enum class LogFormat { Flat, Tree };
  void logNode(std::ostream &stream, const Tree *node, bool recursive,
               bool verbose, int indentation);
  void log(std::ostream &stream, LogFormat format, bool verbose,
           int indentation = 0);
  __attribute__((__used__)) void log() {
    log(std::cout, LogFormat::Tree, false);
  }
#endif
 private:
  Tree::ConstNodes allNodes() {
    return Tree::ConstNodes(Tree::FromBlocks(firstBlock()), numberOfTrees());
  }
  Tree::ConstTrees trees() {
    return Tree::ConstTrees(Tree::FromBlocks(firstBlock()), numberOfTrees());
  }

  void execute(ActionWithContext action, void *context, const void *data,
               int maxSize, Relax relax = k_defaultRelax);

  /* The reference table stores the offset of the tree in the edition pool.
   * - We assume (and assert) that we never referenced more then
   *   k_maxNumberOfTreeRefs at the same time. We make sure of if by
   *   regularly flushing the reference table.
   * - The order of identifiers gives no guarantee on the order of the trees in
   *   the pool.
   */
  class ReferenceTable {
   public:
    /* Special m_identifier when the reference does not point to a Tree yet. */
    constexpr static uint16_t NoNodeIdentifier = 0xFFFF;
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
    bool isFull() { return m_length == EditionPool::k_maxNumberOfReferences; }
    bool reset();
#if POINCARE_TREE_LOG
    void logIdsForNode(std::ostream &stream, const Tree *node) const;
#endif
   private:
    /* Special offset in the nodeOffsetArray when the TreeRef that
     * owned it has been deleted. */
    constexpr static uint16_t DeletedOffset = 0xFFFE;

    uint16_t storeNodeAtIndex(Tree *node, size_t index);

    uint16_t m_length;
    EditionPool *m_pool;
    uint16_t m_nodeOffsetForIdentifier[EditionPool::k_maxNumberOfReferences];
  };

  /* TODO: if we end up needing too many TreeRef, we could ref-count
   * them in m_referenceTable and implement a destructor on TreeRef. */
  ReferenceTable m_referenceTable;
  Block m_blocks[k_maxNumberOfBlocks];
  size_t m_size;
};

#define SharedEditionPool EditionPool::SharedEditionPool

}  // namespace PoincareJ

#endif
