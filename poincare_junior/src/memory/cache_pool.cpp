#include <assert.h>
#include "cache_pool.h"
#include "exception_checkpoint.h"
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
  return idForIndex(Pool::ReferenceTable::storeNodeAtIndex(node, m_length));
}

bool CachePool::ReferenceTable::freeOldestBlocks(int numberOfRequiredFreeBlocks) {
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

void CachePool::ReferenceTable::removeFirstReferences(uint16_t newFirstIndex, Node * nodeToUpdate) {
  // Compute before corrupting the reference table
  size_t cachePoolSize = m_pool->size();
  uint16_t numberOfFreedBlocks = newFirstIndex == m_length ? cachePoolSize : m_nodeOffsetForIdentifier[newFirstIndex];
  memmove(&m_nodeOffsetForIdentifier[0], &m_nodeOffsetForIdentifier[newFirstIndex], sizeof(uint16_t) * (m_length - newFirstIndex));
  m_length -= newFirstIndex;
  m_startIdentifier += newFirstIndex;
  for (int i = 0; i < m_length; i++) {
    m_nodeOffsetForIdentifier[i] -= numberOfFreedBlocks;
  }
  if (nodeToUpdate) {
    *nodeToUpdate = Node(nodeToUpdate->block() - numberOfFreedBlocks);
  }
  static_cast<CachePool *>(m_pool)->translate(numberOfFreedBlocks, cachePoolSize);
}

// CachePool

CachePool * CachePool::sharedCachePool() {
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

bool CachePool::needFreeBlocks(int numberOfBlocks) {
  m_editionPool.flush();
  return numberOfBlocks <= k_maxNumberOfBlocks && m_referenceTable.freeOldestBlocks(numberOfBlocks);
}

void CachePool::reset() {
  m_referenceTable.reset();
  m_editionPool.reinit(lastBlock(), k_maxNumberOfBlocks);
  m_editionPool.flush();
}

int CachePool::execute(ActionWithContext action, void * subAction, const void * data) {
  ExceptionCheckpoint checkpoint;
start_execute:
  if (ExceptionRun(checkpoint)) {
    assert(m_editionPool.numberOfTrees() == 0);
    action(subAction, data);
    // Prevent edition action from leaking: an action create at most one tree
    assert(m_editionPool.numberOfTrees() <= 1);
    return storeEditedTree();
  } else {
    // TODO: assert that we don't delete last called treeForIdentifier otherwise can't copyTreeFromAddress if in cache...
    if (!needFreeBlocks(m_editionPool.fullSize() * 2)) {
      // TODO: try with less demanding reducing context (everything is a float ? SystemTaget?)
      return ReferenceTable::NoNodeIdentifier;
    }
    goto start_execute;
  }
}

CachePool::CachePool() :
  m_referenceTable(this),
  m_editionPool(m_blocks, k_maxNumberOfBlocks)
{
}

void CachePool::resetEditionPool() {
  size_t numberOfCachedBlocks = lastBlock() - firstBlock();
  m_editionPool.reinit(lastBlock(), k_maxNumberOfBlocks - numberOfCachedBlocks);
}

void CachePool::translate(uint16_t offset, size_t cachePoolSize) {
  TypeBlock * newFirst = m_blocks + offset;
  memmove(m_blocks, newFirst, (cachePoolSize + m_editionPool.size()) * sizeof(TypeBlock));
  resetEditionPool();
}

}
