#ifndef POINCARE_MEMORY_CACHE_POOL_H
#define POINCARE_MEMORY_CACHE_POOL_H

#include "cache_reference.h"
#include "edition_pool.h"

namespace Poincare {

class CachePool final : public Pool {
/* The CachePool respects the following assertions:
 * - the referenced addresses are physicaly linear on the pool,
 * - the pool can be fragmented.
 **/
public:
  static CachePool * sharedCachePool();

  const Node nodeForIdentifier(uint16_t id) { return m_referenceTable.nodeForIdentifier(id); }

  uint16_t storeEditedTree();

  EditionPool * editionPool() { return &m_editionPool; }
  bool needFreeBlocks(int numberOfBlocks);
  bool reset();

  int execute(ActionWithContext action, void * subAction, const void * data);

  constexpr static int k_maxNumberOfBlocks = 512;

  TypeBlock * firstBlock() override { return m_referenceTable.isEmpty() ? nullptr : static_cast<TypeBlock *>(&m_pool[0]); }
  TypeBlock * lastBlock() override { return m_referenceTable.isEmpty() ? static_cast<TypeBlock *>(&m_pool[0]) : m_referenceTable.lastNode().nextTree().block(); }
private:
  CachePool();
  void resetEditionPool();

  class ReferenceTable : public Pool::ReferenceTable {
  /* The identifiers are taken from 0 to 2^16 - 2 (0xFFFF being reserved for
   * NoNodeIdentifier).
   * The implementation of the reference table ensures that:
   * - the order of identifiers (% k_maxIdentifier) respects the order of the trees
   * - the oldest trees have smallest identifiers % k_maxIdentifier
   * - the identifiers are allocated on a ring buffer from 0 to 2^16 - 2
   *   (0xFFFF being reserved for NoNodeIdentifier). We keep at maximum
   *   k_maxNumberOfBlocks identifers.
   * - when the identifiers are full, we increment the start of the ring buffer,
   *   invalidating the oldest tree (with the smallest identifier).
   * - when we need free space on the cache, we invalidate the leftest trees
   *   (thereby oldest).
   *
   *  TODO: implement smarter invalidation: don't invalidate the oldest tree but
   *  make a smart choice depending on the last-used date, the time-to-compute,
   *  the age of the tree.
   */
  public:
    ReferenceTable(Pool * pool) : Pool::ReferenceTable(pool) {}
    Node nodeForIdentifier(uint16_t id) const override;
    uint16_t storeNode(Node node) override;
    // Return the address of the new first block
    Block * freeOldestBlocks(int numberOfRequiredFreeBlocks);
    Node lastNode() const;
    bool reset() override;
  private:
    constexpr static uint16_t k_maxIdentifier = UINT16_MAX + 1 - NumberOfSpecialIdentifier;
    uint16_t nextIdentifier() { return idForIndex(m_length); }
    uint16_t idForIndex(uint16_t index) const {
      assert(index < k_maxNumberOfReferences);
      return (static_cast<uint32_t>(m_startIdentifier) + index) % k_maxIdentifier;
    }
    uint16_t indexForId(uint16_t id) const { return (id + k_maxNumberOfReferences - m_startIdentifier) % k_maxNumberOfReferences; }
    void removeFirstReferences(uint16_t newFirstIndex);
    uint16_t m_startIdentifier;
  };

  ReferenceTable m_referenceTable;
  EditionPool m_editionPool;
  Block m_pool[k_maxNumberOfBlocks];
};

}

#endif

