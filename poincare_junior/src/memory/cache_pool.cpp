#include "cache_pool.h"

#include <assert.h>
#include <poincare_junior/include/poincare.h>
#include <string.h>

namespace PoincareJ {

// ReferenceTable

Node CachePool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return Node();
  }
  uint16_t index = indexForId(id);
  if (index == NoNodeIdentifier) {
    return Node();
  }
  return Pool::ReferenceTable::nodeForIdentifier(index);
}

uint16_t CachePool::ReferenceTable::storeNode(Node node) {
  if (isFull()) {
    removeFirstReferences(1, &node);
  }
  uint16_t indexOfNode = Pool::ReferenceTable::storeNodeAtIndex(node, m_length);
#if POINCARE_POOL_VISUALIZATION
  CacheLogger() << "  <Add address=\"" << node.block() << "\">\n";
  CachePool::sharedCachePool()->log(CacheLogger(), Pool::LogFormat::Tree, true,
                                    2);
  CacheLogger() << "\n  </Add>" << std::endl;
#endif
  return idForIndex(indexOfNode);
}

bool CachePool::ReferenceTable::freeOldestBlocks(
    int numberOfRequiredFreeBlocks) {
  int numberOfFreedBlocks = 0;
  uint16_t newFirstIndex;
  for (uint16_t i = 0; i < m_length; i++) {
    Node node = Pool::ReferenceTable::nodeForIdentifier(i);
    numberOfFreedBlocks += node.treeSize();
    if (numberOfFreedBlocks >= numberOfRequiredFreeBlocks) {
      newFirstIndex = i + 1;
      break;
    }
  }
  if (numberOfFreedBlocks < numberOfRequiredFreeBlocks) {
    return false;
  }
  removeFirstReferences(newFirstIndex);
  return true;
}

uint16_t CachePool::ReferenceTable::lastOffset() const {
  assert(!isEmpty());
  return m_nodeOffsetForIdentifier[m_length - 1];
}

bool CachePool::ReferenceTable::reset() {
  m_startIdentifier = 0;
  return Pool::ReferenceTable::reset();
}

void CachePool::ReferenceTable::removeFirstReferences(uint16_t newFirstIndex,
                                                      Node *nodeToUpdate) {
  // Compute before corrupting the reference table
  size_t cachePoolSize = m_pool->size();
  uint16_t numberOfFreedBlocks = newFirstIndex == m_length
                                     ? cachePoolSize
                                     : m_nodeOffsetForIdentifier[newFirstIndex];
  memmove(&m_nodeOffsetForIdentifier[0],
          &m_nodeOffsetForIdentifier[newFirstIndex],
          sizeof(uint16_t) * (m_length - newFirstIndex));
  m_length -= newFirstIndex;
  m_startIdentifier += newFirstIndex;
  for (int i = 0; i < m_length; i++) {
    m_nodeOffsetForIdentifier[i] -= numberOfFreedBlocks;
  }
  if (nodeToUpdate) {
    *nodeToUpdate = Node(nodeToUpdate->block() - numberOfFreedBlocks);
  }
  static_cast<CachePool *>(m_pool)->translate(numberOfFreedBlocks,
                                              cachePoolSize);
#if POINCARE_POOL_VISUALIZATION
  CacheLogger() << "  <Remove numberOfBlocks=\"" << numberOfFreedBlocks
                << "\">\n";
  CachePool::sharedCachePool()->log(CacheLogger(), Pool::LogFormat::Tree, true,
                                    2);
  CacheLogger() << "\n  </Remove>" << std::endl;
#endif
}

// CachePool

CachePool *CachePool::sharedCachePool() {
  static CachePool s_cache;
  return &s_cache;
}

uint16_t CachePool::storeEditedTree() {
  if (m_editionPool.size() == 0) {
    return ReferenceTable::NoNodeIdentifier;
  }
  uint16_t id = m_referenceTable.storeNode(Node(lastBlock()));
  assert(id != ReferenceTable::NoNodeIdentifier);
  resetEditionPool();
  m_editionPool.flush();
  return id;
}

bool CachePool::freeBlocks(int numberOfBlocks, bool flushEditionPool) {
  if (numberOfBlocks > k_maxNumberOfBlocks ||
      !m_referenceTable.freeOldestBlocks(numberOfBlocks)) {
    return false;
  }
  if (flushEditionPool) {
    m_editionPool.flush();
  }
  return true;
}

void CachePool::reset() {
  m_referenceTable.reset();
  m_editionPool.reinit(lastBlock(), k_maxNumberOfBlocks);
  m_editionPool.flush();
#if POINCARE_POOL_VISUALIZATION
  CacheLogger() << "  <Reset>\n";
  CachePool::sharedCachePool()->log(CacheLogger(), Pool::LogFormat::Tree, true,
                                    2);
  CacheLogger() << "\n  </Reset>" << std::endl;
#endif
}

CachePool::CachePool()
    : m_referenceTable(this),
      m_editionPool(static_cast<TypeBlock *>(static_cast<Block *>(m_blocks)),
                    k_maxNumberOfBlocks) {}

void CachePool::resetEditionPool() {
  m_editionPool.reinit(lastBlock(), k_maxNumberOfBlocks - size());
}

void CachePool::translate(uint16_t offset, size_t cachePoolSize) {
  Block *newFirst = m_blocks + offset;
  memmove(m_blocks, newFirst,
          (cachePoolSize + m_editionPool.size()) * sizeof(TypeBlock));
  resetEditionPool();
}

}  // namespace PoincareJ
