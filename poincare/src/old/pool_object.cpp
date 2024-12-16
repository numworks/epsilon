#include <poincare/old/pool.h>
#include <poincare/old/pool_handle.h>
#include <poincare/old/pool_object.h>

namespace Poincare {

// Node operations

void PoolObject::release(int currentNumberOfChildren) {
  if (!isAfterTopmostCheckpoint()) {
    /* Do not decrease reference counters outside of the current checkpoint
     * since they were not increased. */
    return;
  }
  m_referenceCounter--;
  if (m_referenceCounter == 0) {
    deleteParentIdentifierInChildren();
    Pool::sharedPool->removeChildrenAndDestroy(this, currentNumberOfChildren);
  }
}

void PoolObject::rename(uint16_t identifier, bool unregisterPreviousIdentifier,
                        bool skipChildrenUpdate) {
  if (unregisterPreviousIdentifier) {
    /* The previous identifier should not always be unregistered. For instance,
     * if the node is a clone and still has the original node's identifier,
     * unregistering it would lose the access to the original node. */
    Pool::sharedPool->unregisterNode(this);
  }
  m_identifier = identifier;
  m_referenceCounter = 0;
  Pool::sharedPool->registerNode(this);
  if (skipChildrenUpdate) {
    return;
  }
  updateParentIdentifierInChildren();
}

// Hierarchy

PoolObject *PoolObject::nextSibling() const {
  int remainingNodesToVisit = numberOfChildren();
  PoolObject *node = const_cast<PoolObject *>(this)->next();
  while (remainingNodesToVisit > 0) {
    remainingNodesToVisit += node->numberOfChildren();
    node = node->next();
    remainingNodesToVisit--;
  }
  return node;
}

// Protected

#if POINCARE_TREE_LOG
void PoolObject::log(std::ostream &stream, bool recursive, int indentation,
                     bool verbose) {
  stream << "\n";
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
  stream << "<";
  logNodeName(stream);
  if (verbose) {
    stream << " id=\"" << m_identifier << "\"";
    stream << " refCount=\"" << (int16_t)m_referenceCounter << "\"";
    stream << " size=\"" << size() << "\"";
  }
  logAttributes(stream);
  bool tagIsClosed = false;
  if (recursive) {
    for (PoolObject *child : directChildren()) {
      if (!tagIsClosed) {
        stream << ">";
        tagIsClosed = true;
      }
      child->log(stream, recursive, indentation + 1, verbose);
    }
  }
  if (tagIsClosed) {
    stream << "\n";
    for (int i = 0; i < indentation; ++i) {
      stream << "  ";
    }
    stream << "</";
    logNodeName(stream);
    stream << ">";
  } else {
    stream << "/>";
  }
}
#endif

size_t PoolObject::deepSize(int realNumberOfChildren) const {
  if (realNumberOfChildren == -1) {
    return reinterpret_cast<char *>(nextSibling()) -
           reinterpret_cast<const char *>(this);
  }
  PoolObject *realNextSibling = next();
  for (int i = 0; i < realNumberOfChildren; i++) {
    realNextSibling = realNextSibling->nextSibling();
  }
  return reinterpret_cast<char *>(realNextSibling) -
         reinterpret_cast<const char *>(this);
}

void PoolObject::changeParentIdentifierInChildren(uint16_t id) const {
  for (PoolObject *c : directChildren()) {
    c->setParentIdentifier(id);
  }
}

}  // namespace Poincare
