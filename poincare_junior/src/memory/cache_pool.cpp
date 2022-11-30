#include <assert.h>
#include "cache_pool.h"
#include "exception_checkpoint.h"
#include <string.h>

namespace Poincare {

// ReferenceTable

Node CachePool::ReferenceTable::nodeForIdentifier(uint16_t id) const {
  if (id == NoNodeIdentifier) {
    return Node();
  }
  uint16_t index = indexForId(id);
  if (index >= m_length) {
    return Node();
  }
  return Pool::ReferenceTable::nodeForIdentifier(index);
}

uint16_t CachePool::ReferenceTable::storeNode(Node node) {
  if (isFull()) {
    m_startIdentifier++;
    removeFirstReferences(1);
  }
  return idForIndex(Pool::ReferenceTable::storeNode(node));
}

Block * CachePool::ReferenceTable::freeOldestBlocks(int numberOfRequiredFreeBlocks) {
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
    return nullptr;
  }
  removeFirstReferences(newFirstIndex);
  for (int i = 0; i < m_length; i++) {
    m_nodeOffsetForIdentifier[i] -= numberOfFreedBlocks;
  }
  return Pool::ReferenceTable::nodeForIdentifier(0).block();
}

Node CachePool::ReferenceTable::lastNode() const {
  return Pool::ReferenceTable::nodeForIdentifier(m_length - 1);
}

bool CachePool::ReferenceTable::reset() {
  m_startIdentifier = 0;
  return Pool::ReferenceTable::reset();
}

void CachePool::ReferenceTable::removeFirstReferences(uint16_t newFirstIndex) {
  memmove(&m_nodeOffsetForIdentifier[0], &m_nodeOffsetForIdentifier[newFirstIndex], sizeof(uint16_t) * (m_length - newFirstIndex));
  m_length -= newFirstIndex;
}

// CachePool

CachePool * CachePool::sharedCachePool() {
  static CachePool s_cache;
  return &s_cache;
}

uint16_t CachePool::storeEditedTree() {
  if (m_editionPool.numberOfBlocks() == 0) {
    return ReferenceTable::NoNodeIdentifier;
  }
  uint16_t id = m_referenceTable.storeNode(Node(lastBlock()));
  assert(id != ReferenceTable::NoNodeIdentifier);
  resetEditionPool();
  return id;
}

bool CachePool::needFreeBlocks(int numberOfBlocks) {
  if (numberOfBlocks > k_maxNumberOfBlocks) {
    return false;
  }
  Block * newFirstBlock = m_referenceTable.freeOldestBlocks(numberOfBlocks);
  if (newFirstBlock == nullptr) {
    return false;
  }
  size_t numberOfCachedBlocks = lastBlock() - firstBlock();
  memmove(m_pool, newFirstBlock, numberOfCachedBlocks * sizeof(Block));
  resetEditionPool();
  return true;
}

bool CachePool::reset() {
  if (m_referenceTable.isEmpty()) {
    // The cache has already been emptied
    return false;
  }
  m_referenceTable.reset();
  m_editionPool.reinit(lastBlock(), k_maxNumberOfBlocks);
  return true;
}

int CachePool::execute(ActionWithContext action, void * subAction, const void * data) {
  ExceptionCheckpoint checkpoint;
start_execute:
  if (ExceptionRun(checkpoint)) {
    m_editionPool.flush();
    action(subAction, data);
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
  m_editionPool(static_cast<TypeBlock *>(&m_pool[0]), k_maxNumberOfBlocks)
{
}

void CachePool::resetEditionPool() {
  size_t numberOfCachedBlocks = lastBlock() - firstBlock();
  m_editionPool.reinit(lastBlock(), k_maxNumberOfBlocks - numberOfCachedBlocks);
}

}
