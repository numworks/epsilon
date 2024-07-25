#ifndef POINCARE_MEMORY_BLOCK_STACK_H
#define POINCARE_MEMORY_BLOCK_STACK_H

#include <string.h>

#include "type_block.h"
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare::Internal {

/* A stack of Blocks (= bytes) with motion tracking references. */

class BlockStack {
 public:
  constexpr static int k_maxNumberOfBlocks = 1024 * 16;
  constexpr static int k_maxNumberOfReferences = k_maxNumberOfBlocks / 8;

  BlockStack() : m_referenceTable(this), m_size(0) {}

  const Block* firstBlock() const { return m_blocks; }
  Block* firstBlock() { return m_blocks; }
  // If BlockStack is empty, first and last blocks are the same one
  const Block* lastBlock() const { return m_blocks + m_size; }
  Block* lastBlock() { return m_blocks + m_size; }
  size_t size() const { return m_size; }
  Block* blockAtIndex(int i) { return firstBlock() + i; }

  bool contains(const Block* block) const {
    return block >= firstBlock() && block < lastBlock();
  }
  // Return true if block is the root node of one of the pool's trees.
  bool isRootBlock(const Block* block, bool allowLast = false) const;

  // Initialize trees
  Block* initFromAddress(const void* address, bool isTree = true);

  void replaceBlock(Block* previousBlock, Block newBlock);
  void replaceBlocks(Block* destination, const Block* newBlocks,
                     size_t numberOfBlocks);
  Block* pushBlock(Block block) {
    insertBlock(lastBlock(), block, true);
    return lastBlock() - 1;
  }

  bool insertBlock(Block* destination, Block block, bool at = false) {
    return insertBlocks(destination, &block, 1, at);
  }
  bool insertBlocks(Block* destination, const Block* source,
                    size_t numberOfBlocks, bool at = false);
  void popBlock() { removeBlocks(lastBlock() - 1, 1); }
  void removeBlocks(Block* address, size_t numberOfBlocks);
  void moveBlocks(Block* destination, Block* source, size_t numberOfBlocks,
                  bool at = false);

  void flush();
  void flushFromBlock(const Block* node);
  uint16_t referenceBlock(Block* node);
  void deleteIdentifier(uint16_t id) { m_referenceTable.deleteIdentifier(id); }
  void updateIdentifier(uint16_t id, Block* newNode) {
    m_referenceTable.updateIdentifier(id, newNode);
  }
  Block* blockForIdentifier(uint16_t id) {
    return m_referenceTable.nodeForIdentifier(id);
  }

  /* We delete the assignment operator because copying without care the
   * ReferenceTable would corrupt the m_referenceTable.m_pool pointer. */
  BlockStack& operator=(BlockStack&&) = delete;
  BlockStack& operator=(const BlockStack&) = delete;

 protected:
  /* The reference table stores the offset of the tree in the edition pool.
   * - We assume (and assert) that we never referenced more then
   *   k_maxNumberOfTreeRefs at the same time. We make sure of if by
   *   regularly flushing the reference table.
   * - The order of identifiers gives no guarantee on the order of the trees
   * in the pool.
   */
  class ReferenceTable {
   public:
    /* Special m_identifier when the reference does not point to a Tree yet. */
    constexpr static uint16_t NoNodeIdentifier = 0xFFFF;
    /* Special offset in the nodeOffsetArray when the pointed Tree has been
     * removed or replaced. */
    constexpr static uint16_t InvalidatedOffset = 0xFFFF;

    ReferenceTable(BlockStack* pool) : m_length(0), m_pool(pool) {}
    Block* nodeForIdentifier(uint16_t id) const;
    uint16_t storeNode(Block* node);
    void updateIdentifier(uint16_t id, Block* newNode);
    void deleteIdentifier(uint16_t id);
    typedef void (*AlterSelectedBlock)(uint16_t*, Block*, const Block*,
                                       const Block*, int);
    void updateNodes(AlterSelectedBlock function,
                     const Block* contextSelection1,
                     const Block* contextSelection2, int contextAlteration);
    void invalidateIdentifiersAfterBlock(const Block* block);
    bool isFull() { return m_length == BlockStack::k_maxNumberOfReferences; }
    bool reset();
#if POINCARE_TREE_LOG
    void logIdsForNode(std::ostream& stream, const Block* node) const;
#endif
   private:
    /* Special offset in the nodeOffsetArray when the TreeRef that
     * owned it has been deleted. */
    constexpr static uint16_t DeletedOffset = 0xFFFE;

    uint16_t storeNodeAtIndex(Block* node, size_t index);

    uint16_t m_length;
    BlockStack* m_pool;
    uint16_t m_nodeOffsetForIdentifier[BlockStack::k_maxNumberOfReferences];
  };

  /* If we end up needing too many TreeRef, we could ref-count  them in
   * m_referenceTable and implement a destructor on TreeRef. */
  ReferenceTable m_referenceTable;
  Block m_blocks[k_maxNumberOfBlocks];
  size_t m_size;
};

}  // namespace Poincare::Internal

#endif
