#include <poincare/pool.h>
#include <poincare/pool_object.h>
#include <poincare/tree_handle.h>

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

PoolObject *PoolObject::parent() const {
  assert(m_parentIdentifier != m_identifier);
  return PoolHandle::hasNode(m_parentIdentifier)
             ? Pool::sharedPool->node(m_parentIdentifier)
             : nullptr;
}

PoolObject *PoolObject::root() {
  PoolObject *result = this;
  PoolObject *resultParent = result->parent();
  while (resultParent != nullptr) {
    result = resultParent;
    resultParent = result->parent();
  }
  return result;
}

int PoolObject::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  PoolObject *nextSiblingNode = nextSibling();
  PoolObject *currentNode = next();
  while (currentNode != nextSiblingNode) {
    result++;
    currentNode = currentNode->next();
  }
  return result;
}

PoolObject *PoolObject::childAtIndex(int i) const {
  assert(i >= 0);
  assert(i < numberOfChildren());
  PoolObject *child = next();
  while (i > 0) {
    child = child->nextSibling();
    assert(child != nullptr);
    i--;
  }
  assert(m_identifier != child->identifier());
  return child;
}

int PoolObject::indexOfChild(const PoolObject *child) const {
  assert(child != nullptr);
  int childrenCount = numberOfChildren();
  PoolObject *childAtIndexi = next();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndexi == child) {
      return i;
    }
    childAtIndexi = childAtIndexi->nextSibling();
  }
  return -1;
}

int PoolObject::indexInParent() const {
  PoolObject *p = parent();
  if (p == nullptr) {
    return -1;
  }
  return p->indexOfChild(this);
}

bool PoolObject::hasChild(const PoolObject *child) const {
  for (PoolObject *c : directChildren()) {
    if (child == c) {
      return true;
    }
  }
  return false;
}

bool PoolObject::hasAncestor(const PoolObject *node, bool includeSelf) const {
  if (includeSelf && node == this) {
    return true;
  }
  for (PoolObject *t : node->depthFirstChildren()) {
    if (this == t) {
      return true;
    }
  }
  return false;
}

bool PoolObject::hasSibling(const PoolObject *e) const {
  PoolObject *p = parent();
  if (p == nullptr) {
    return false;
  }
  for (PoolObject *childNode : p->directChildren()) {
    if (childNode == e) {
      return true;
    }
  }
  return false;
}

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

PoolObject *PoolObject::lastDescendant() const {
  PoolObject *node = const_cast<PoolObject *>(this);
  int remainingNodesToVisit = node->numberOfChildren();
  while (remainingNodesToVisit > 0) {
    node = node->next();
    remainingNodesToVisit--;
    remainingNodesToVisit += node->numberOfChildren();
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

bool PoolObject::deepIsGhost() const {
  if (isGhost()) {
    return true;
  }
  for (PoolObject *c : directChildren()) {
    if (c->deepIsGhost()) {
      return true;
    }
  }
  return false;
}

void PoolObject::changeParentIdentifierInChildren(uint16_t id) const {
  for (PoolObject *c : directChildren()) {
    c->setParentIdentifier(id);
  }
}

}  // namespace Poincare
