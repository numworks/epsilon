#include <omg/memory.h>
#include <poincare/old/ghost.h>
#include <poincare/old/pool_handle.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Clone */

PoolHandle PoolHandle::clone() const {
  assert(!isUninitialized());
  PoolObject *nodeCopy = Pool::sharedPool->deepCopy(object());
  nodeCopy->deleteParentIdentifier();
  return PoolHandle(nodeCopy);
}

/* Hierarchy operations */
PoolObject *PoolHandle::object() const {
  assert(hasNode(m_identifier));
  return Pool::sharedPool->node(m_identifier);
}

size_t PoolHandle::size() const {
  return object()->deepSize(object()->numberOfChildren());
}

#if POINCARE_TREE_LOG
void PoolHandle::log() const {
  if (!isUninitialized()) {
    return object()->log();
  }
  std::cout << "\n<Uninitialized PoolHandle/>" << std::endl;
}
#endif

/* Private */

PoolHandle::PoolHandle(const PoolObject *node) : PoolHandle() {
  if (node != nullptr) {
    setIdentifierAndRetain(node->identifier());
  }
}

template <class U>
PoolHandle PoolHandle::Builder() {
  void *bufferNode = Pool::sharedPool->alloc(sizeof(U));
  U *node = new (bufferNode) U();
  return PoolHandle::BuildWithGhostChildren(node);
}

PoolHandle PoolHandle::BuildWithGhostChildren(PoolObject *node) {
  assert(node != nullptr);
  Pool *pool = Pool::sharedPool;
  int expectedNumberOfChildren = node->numberOfChildren();
  /* Ensure the pool is syntaxically correct by creating ghost children for
   * nodes that have a fixed, non-zero number of children. */
  uint16_t nodeIdentifier = pool->generateIdentifier();
  node->rename(nodeIdentifier, false, true);
  for (int i = 0; i < expectedNumberOfChildren; i++) {
    GhostNode *ghost = new (pool->alloc(sizeof(GhostNode))) GhostNode();
    ghost->rename(pool->generateIdentifier(), false);
    ghost->setParentIdentifier(nodeIdentifier);
    ghost->retain();
    assert((char *)ghost ==
           (char *)node->next() +
               i * OMG::Memory::AlignedSize(sizeof(GhostNode), ByteAlignment));
  }
  return PoolHandle(node);
}

void PoolHandle::setIdentifierAndRetain(uint16_t newId) {
  m_identifier = newId;
  if (!isUninitialized()) {
    object()->retain();
  }
}

void PoolHandle::setTo(const PoolHandle &tr) {
  /* We cannot use (*this)==tr because tr would need to be casted to
   * PoolHandle, which calls setTo and triggers an infinite loop */
  if (identifier() == tr.identifier()) {
    return;
  }
  int currentId = identifier();
  setIdentifierAndRetain(tr.identifier());
  release(currentId);
}

void PoolHandle::release(uint16_t identifier) {
  if (!hasNode(identifier)) {
    return;
  }
  PoolObject *node = Pool::sharedPool->node(identifier);
  if (node == nullptr) {
    /* The identifier is valid, but not the node: there must have been an
     * exception that deleted the pool. */
    return;
  }
  assert(node->identifier() == identifier);
  node->release(node->numberOfChildren());
}

}  // namespace Poincare
