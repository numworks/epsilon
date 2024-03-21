#ifndef POINCARE_MEMORY_CACHE_POOL_H
#define POINCARE_MEMORY_CACHE_POOL_H

#include <omg/global_box.h>

#include "edition_pool.h"
#include "reference.h"

namespace PoincareJ {

/*  <---- EditionPool ----> <------------------- CachePool ------------------>
 * +-----------------------+-------------------------+---+--------------------+
 * | draft trees           | most recent cached tree | … | oldest cached tree |
 * +-----------------------+-------------------------+---+--------------------+
 * ^ m_blocks              ^ nodeOffset[m_length-1]      ^ nodeOffset[0]
 */

class CachePool final : public Pool {
  friend class OMG::GlobalBox<CachePool>;
  friend class Reference;
  /* The CachePool respects the following assertions:
   * - the referenced addresses are physically linear on the pool,
   * - the pool can be fragmented.
   **/
 public:
  static OMG::GlobalBox<CachePool> SharedCachePool;

  uint16_t storeEditedTree();

  bool freeBlocks(int numberOfBlocks);
  /* reset should be used when all CacheReference have been destroyed to ensure
   * that they won't point to reallocated nodes */
  void reset();

  using Pool::firstBlock;
  const Block *firstBlock() const override {
    return m_referenceTable.isEmpty()
               ? lastBlock()
               : m_blocks + m_referenceTable.firstOffset();
  }
  using Pool::lastBlock;
  // If CachePool is empty, first and last blocks are the same one
  const Block *lastBlock() const override {
    return m_blocks + k_maxNumberOfBlocks;
  }

  Block *referenceBlock() override { return m_blocks; }

  // Broader implementation of Pool::contains, checking unused pool as well
  bool mayContain(const Block *block) const {
    return block >= m_blocks && block < m_blocks + k_maxNumberOfBlocks;
  }

  // TODO: not all pool sizes are passing the tests, investigate why
  constexpr static int k_maxNumberOfBlocks = 1024 * 16;
  constexpr static int k_maxNumberOfReferences = k_maxNumberOfBlocks / 8;

 private:
  CachePool();
  void translate(uint16_t offset, Block *start);
  void resizeEditionPool();
#if POINCARE_TREE_LOG
  const char *name() override { return "Cache"; }
#endif

  class ReferenceTable : public Pool::ReferenceTable {
    /* The identifiers are taken from 0 to 2^16 - 2 (0xFFFF being reserved for
     * NoNodeIdentifier).
     * The implementation of the reference table ensures that:
     * - the order of identifiers respects the order of the trees
     * - the oldest trees have smallest identifiers
     * - we never reach the k_maxIdentifier = 2^16 - 1
     *   (0xFFFF being reserved for NoNodeIdentifier)
     * - we keep at maximum k_maxNumberOfBlocks identifers.
     * - when the identifiers are full, we increment the start recognized
     * identifier, invalidating the oldest tree (with the smallest identifier).
     * - when we need free space on the cache, we invalidate the leftest trees
     *   (thereby oldest).
     *
     *  TODO: implement smarter invalidation: don't invalidate the oldest tree
     * but make a smart choice depending on the last-used date, the
     * time-to-compute, the age of the tree.
     */
   public:
    ReferenceTable(Pool *pool) : Pool::ReferenceTable(pool) {}
    Tree *nodeForIdentifier(uint16_t id) const override;
    uint16_t storeNode(Tree *node) override;
    // Returns a boolean indicating if we can free numberOfRequiredFreeBlocks
    bool freeOldestBlocks(int numberOfRequiredFreeBlocks);
    uint16_t firstOffset() const;
    bool reset() override;
    void removeLastReferences(uint16_t newFirstIndex);

   private:
    size_t maxNumberOfReferences() const override {
      return CachePool::k_maxNumberOfReferences;
    }
    uint16_t *nodeOffsetArray() override { return m_nodeOffsetForIdentifier; }
#if POINCARE_TREE_LOG
    uint16_t identifierForIndex(uint16_t index) const override {
      return idForIndex(index);
    }
#endif
    constexpr static uint16_t k_maxIdentifier =
        UINT16_MAX - NumberOfSpecialIdentifier;
    static_assert(NoNodeIdentifier == UINT16_MAX,
                  "Special identifier is not the last taken identifier");
    uint16_t nextIdentifier() { return idForIndex(m_length); }
    uint16_t idForIndex(uint16_t index) const {
      assert(index < k_maxNumberOfReferences);
      assert(static_cast<uint32_t>(m_startIdentifier) + index <=
             k_maxIdentifier);
      return static_cast<uint32_t>(m_startIdentifier) + index;
    }
    uint16_t indexForId(uint16_t id) const {
      assert(static_cast<uint32_t>(m_startIdentifier) + m_length <=
             k_maxIdentifier);
      if (id < m_startIdentifier || id >= m_startIdentifier + m_length) {
        return NoNodeIdentifier;
      }
      return id - m_startIdentifier;
    }
    uint16_t m_nodeOffsetForIdentifier[CachePool::k_maxNumberOfReferences];
    uint16_t m_startIdentifier;
  };
  const ReferenceTable *referenceTable() const override {
    return &m_referenceTable;
  }

  ReferenceTable m_referenceTable;
  Block m_blocks[k_maxNumberOfBlocks];
};

}  // namespace PoincareJ

#endif
