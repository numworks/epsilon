#include <poincare/checkpoint.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/helpers.h>
#include <poincare/pool.h>
#include <poincare/pool_handle.h>
#include <stdint.h>
#include <string.h>

namespace Poincare {

#if ASSERTIONS
bool Pool::s_treePoolLocked = false;
#endif

OMG::GlobalBox<Pool> Pool::sharedPool;

void Pool::freeIdentifier(uint16_t identifier) {
  if (PoolObject::IsValidIdentifier(identifier) &&
      identifier < MaxNumberOfNodes) {
    m_nodeForIdentifierOffset[identifier] = UINT16_MAX;
    m_identifiers.push(identifier);
  }
}

void Pool::move(PoolObject *destination, PoolObject *source,
                int realNumberOfSourceChildren) {
  size_t moveSize = source->deepSize(realNumberOfSourceChildren);
  moveNodes(destination, source, moveSize);
}

void Pool::moveChildren(PoolObject *destination, PoolObject *sourceParent) {
  size_t moveSize = sourceParent->deepSize(-1) -
                    Helpers::AlignedSize(sourceParent->size(), ByteAlignment);
  moveNodes(destination, sourceParent->next(), moveSize);
}

void Pool::removeChildren(PoolObject *node, int nodeNumberOfChildren) {
  for (int i = 0; i < nodeNumberOfChildren; i++) {
    PoolObject *child = node->childAtIndex(0);
    int childNumberOfChildren = child->numberOfChildren();
    /* The new child will be put at the address last(), but removed from its
     * previous position, hence the newAddress we use. */
    PoolObject *newAddress =
        (PoolObject *)((char *)last() -
                       (char *)child->deepSize(childNumberOfChildren));
    move(last(), child, childNumberOfChildren);
    newAddress->release(newAddress->numberOfChildren());
  }
  node->eraseNumberOfChildren();
}

PoolObject *Pool::deepCopy(PoolObject *node) {
  size_t size = node->deepSize(-1);
  return copyTreeFromAddress(static_cast<void *>(node), size);
}

PoolObject *Pool::copyTreeFromAddress(const void *address, size_t size) {
  void *ptr = alloc(size);
  memcpy(ptr, address, size);
  PoolObject *copy = reinterpret_cast<PoolObject *>(ptr);
  renameNode(copy, false);
  for (PoolObject *child : copy->depthFirstChildren()) {
    renameNode(child, false);
    child->retain();
  }
  return copy;
}

void Pool::removeChildrenAndDestroy(PoolObject *nodeToDestroy,
                                    int nodeNumberOfChildren) {
  removeChildren(nodeToDestroy, nodeNumberOfChildren);
  discardPoolObject(nodeToDestroy);
}

void Pool::moveNodes(PoolObject *destination, PoolObject *source,
                     size_t moveSize) {
  assert(destination->isAfterTopmostCheckpoint());
  assert(source->isAfterTopmostCheckpoint());
  assert(moveSize % 4 == 0);
  assert((((uintptr_t)source) % 4) == 0);
  assert((((uintptr_t)destination) % 4) == 0);

#if ASSERTIONS
  assert(!s_treePoolLocked);
#endif

  uint32_t *src = reinterpret_cast<uint32_t *>(source);
  uint32_t *dst = reinterpret_cast<uint32_t *>(destination);
  size_t len = moveSize / 4;

  if (Helpers::Rotate(dst, src, len)) {
    updateNodeForIdentifierFromNode(dst < src ? destination : source);
  }
}

#if POINCARE_TREE_LOG
void Pool::flatLog(std::ostream &stream) {
  size_t size = static_cast<char *>(m_cursor) - static_cast<char *>(buffer());
  stream << "<Pool format=\"flat\" size=\"" << size << "\">";
  for (PoolObject *node : allNodes()) {
    node->log(stream, false);
  }
  stream << "</Pool>";
  stream << std::endl;
}

void Pool::treeLog(std::ostream &stream, bool verbose) {
  stream << "<Pool format=\"tree\" size=\"" << (int)(m_cursor - buffer())
         << "\">";
  for (PoolObject *node : roots()) {
    node->log(stream, true, 1, verbose);
  }
  stream << std::endl;
  stream << "</Pool>";
  stream << std::endl;
}

#endif

int Pool::numberOfNodes() const {
  int count = 0;
  PoolObject *firstNode = first();
  PoolObject *lastNode = last();
  while (firstNode != lastNode) {
    count++;
    firstNode = firstNode->next();
  }
  return count;
}

void *Pool::alloc(size_t size) {
  assert(last()->isAfterTopmostCheckpoint());
#if ASSERTIONS
  assert(!s_treePoolLocked);
#endif

  size = Helpers::AlignedSize(size, ByteAlignment);
  if (m_cursor + size > buffer() + BufferSize) {
    ExceptionCheckpoint::Raise();
  }
  void *result = m_cursor;
  m_cursor += size;
  return result;
}

void Pool::dealloc(PoolObject *node, size_t size) {
  assert(node->isAfterTopmostCheckpoint());
#if ASSERTIONS
  assert(!s_treePoolLocked);
#endif

  size = Helpers::AlignedSize(size, ByteAlignment);
  char *ptr = reinterpret_cast<char *>(node);
  assert(ptr >= buffer() && ptr < m_cursor);

  // Step 1 - Compact the pool
  memmove(ptr, ptr + size, m_cursor - (ptr + size));
  m_cursor -= size;

  // Step 2: Update m_nodeForIdentifierOffset for all nodes downstream
  updateNodeForIdentifierFromNode(node);
}

void Pool::discardPoolObject(PoolObject *node) {
  uint16_t nodeIdentifier = node->identifier();
  size_t size = node->size();
  node->~PoolObject();
  dealloc(node, size);
  freeIdentifier(nodeIdentifier);
}

void Pool::registerNode(PoolObject *node) {
  uint16_t nodeID = node->identifier();
  assert(nodeID < MaxNumberOfNodes);
  const int nodeOffset =
      (((char *)node) - (char *)m_alignedBuffer) / ByteAlignment;
  // Check that the offset can be stored in a uint16_t
  assert(nodeOffset < k_maxNodeOffset);
  m_nodeForIdentifierOffset[nodeID] = nodeOffset;
}

void Pool::updateNodeForIdentifierFromNode(PoolObject *node) {
  for (PoolObject *n : Nodes(node)) {
    registerNode(n);
  }
}

// Reset IdentifierStack, make all identifiers available
void Pool::IdentifierStack::reset() {
  for (uint16_t i = 0; i < MaxNumberOfNodes; i++) {
    m_availableIdentifiers[i] = i;
  }
  m_currentIndex = MaxNumberOfNodes;
}

void Pool::IdentifierStack::push(uint16_t i) {
  assert(PoolObject::IsValidIdentifier(m_currentIndex) &&
         m_currentIndex < MaxNumberOfNodes);
  m_availableIdentifiers[m_currentIndex++] = i;
}

uint16_t Pool::IdentifierStack::pop() {
  if (m_currentIndex == 0) {
    assert(false);
    return 0;
  }
  assert(m_currentIndex > 0 && m_currentIndex <= MaxNumberOfNodes);
  return m_availableIdentifiers[--m_currentIndex];
}

// Remove an available identifier.
void Pool::IdentifierStack::remove(uint16_t j) {
  assert(PoolObject::IsValidIdentifier(j));
  /* TODO: Implement an optimized binary search using the sorted state.
   * Alternatively, it may be worth using another data type such as a sorted
   * list instead of a stack. */
  for (uint16_t i = 0; i < m_currentIndex; i++) {
    if (m_availableIdentifiers[i] == j) {
      m_availableIdentifiers[i] = m_availableIdentifiers[--m_currentIndex];
      return;
    }
  }
  assert(false);
}

// Reset m_nodeForIdentifierOffset for all available identifiers
void Pool::IdentifierStack::resetNodeForIdentifierOffsets(
    uint16_t *nodeForIdentifierOffset) const {
  for (uint16_t i = 0; i < m_currentIndex; i++) {
    nodeForIdentifierOffset[m_availableIdentifiers[i]] = UINT16_MAX;
  }
}

// Discard all nodes after firstNodeToDiscard
void Pool::freePoolFromNode(PoolObject *firstNodeToDiscard) {
  assert(firstNodeToDiscard != nullptr);
  assert(firstNodeToDiscard >= first());
  assert(firstNodeToDiscard <= last());

  // Free all identifiers
  m_identifiers.reset();
  PoolObject *currentNode = first();
  while (currentNode < firstNodeToDiscard) {
    m_identifiers.remove(currentNode->identifier());
    currentNode = currentNode->next();
  }
  assert(currentNode == firstNodeToDiscard);
  m_identifiers.resetNodeForIdentifierOffsets(m_nodeForIdentifierOffset);
  m_cursor = reinterpret_cast<char *>(currentNode);
  // TODO: Assert that no tree continues into the discarded pool zone
}

}  // namespace Poincare
