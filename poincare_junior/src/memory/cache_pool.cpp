#include "cache_pool.h"

#include <assert.h>
#include <poincare_junior/include/poincare.h>
#include <string.h>

namespace PoincareJ {

// ReferenceTable

Tree *CachePool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return nullptr;
  }
  uint16_t index = indexForId(id);
  if (index == NoNodeIdentifier) {
    return nullptr;
  }
  return Pool::ReferenceTable::nodeForIdentifier(index);
}

uint16_t CachePool::ReferenceTable::storeNode(Tree *node) {
  assert(!isFull());
  uint16_t indexOfNode = Pool::ReferenceTable::storeNodeAtIndex(node, m_length);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Cache, "Add", node->block(), node->treeSize());
#endif
  return idForIndex(indexOfNode);
}

bool CachePool::ReferenceTable::freeOldestBlocks(
    int numberOfRequiredFreeBlocks) {
  int numberOfFreedBlocks = 0;
  uint16_t newFirstIndex;
  for (uint16_t i = 0; i < m_length; i++) {
    Tree *node = Pool::ReferenceTable::nodeForIdentifier(i);
    numberOfFreedBlocks += node->treeSize();
    if (numberOfFreedBlocks >= numberOfRequiredFreeBlocks) {
      newFirstIndex = i + 1;
      break;
    }
  }
  if (numberOfFreedBlocks < numberOfRequiredFreeBlocks) {
    return false;
  }
  removeLastReferences(newFirstIndex);
  return true;
}

uint16_t CachePool::ReferenceTable::firstOffset() const {
  assert(!isEmpty());
  return m_nodeOffsetForIdentifier[m_length - 1];
}

bool CachePool::ReferenceTable::reset() {
  m_startIdentifier = 0;
  return Pool::ReferenceTable::reset();
}

void CachePool::ReferenceTable::removeLastReferences(uint16_t newFirstIndex) {
  // Compute before corrupting the reference table
  size_t cachePoolSize = m_pool->size();
  uint16_t numberOfFreedBlocks =
      newFirstIndex == m_length
          ? cachePoolSize
          : k_maxNumberOfBlocks - m_nodeOffsetForIdentifier[newFirstIndex - 1];
  memmove(&m_nodeOffsetForIdentifier[0],
          &m_nodeOffsetForIdentifier[newFirstIndex],
          sizeof(uint16_t) * (m_length - newFirstIndex));
  m_length -= newFirstIndex;
  m_startIdentifier += newFirstIndex;
  for (int i = 0; i < m_length; i++) {
    m_nodeOffsetForIdentifier[i] += numberOfFreedBlocks;
  }
  static_cast<CachePool *>(m_pool)->translate(
      numberOfFreedBlocks, m_pool->lastBlock() - cachePoolSize);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Cache, "Remove", m_pool->firstBlock(), numberOfFreedBlocks);
#endif
}

// CachePool

OMG::GlobalBox<CachePool> CachePool::SharedCachePool;

uint16_t CachePool::storeEditedTree() {
  assert(SharedEditionPool->numberOfTrees() == 1);
  if (m_referenceTable.isFull()) {
    m_referenceTable.removeLastReferences(1);
  }
  Tree *tree = Tree::FromBlocks(m_blocks);
  size_t len = tree->treeSize();
  Tree *dest = Tree::FromBlocks(firstBlock() - len);
  // Calling EditionPool move would edit EditionReferences
  memmove(dest, tree, len);
  uint16_t id = m_referenceTable.storeNode(dest);
  assert(id != ReferenceTable::NoNodeIdentifier);
  SharedEditionPool->flush();
  resizeEditionPool();
  return id;
}

bool CachePool::freeBlocks(int numberOfBlocks) {
  if (numberOfBlocks > k_maxNumberOfBlocks ||
      !m_referenceTable.freeOldestBlocks(numberOfBlocks)) {
    return false;
  }
  return true;
}

void CachePool::reset() {
  m_referenceTable.reset();
  SharedEditionPool->setSize(k_maxNumberOfBlocks);
  SharedEditionPool->flush();
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Cache, "Flush");
#endif
}

CachePool::CachePool() : m_referenceTable(this) {
  EditionPool::SharedEditionPool.init(
      static_cast<TypeBlock *>(static_cast<Block *>(m_blocks)),
      k_maxNumberOfBlocks);
}

void CachePool::resizeEditionPool() {
  SharedEditionPool->setSize(k_maxNumberOfBlocks - size());
}

void CachePool::translate(uint16_t offset, Block *start) {
  memmove(start + offset, start, lastBlock() - start - offset);
  resizeEditionPool();
}

}  // namespace PoincareJ
