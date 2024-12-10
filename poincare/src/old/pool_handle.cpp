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

PoolHandle PoolHandle::parent() const {
  return (isUninitialized() || object()->parent() == nullptr)
             ? PoolHandle()
             : PoolHandle(object()->parent());
}

int PoolHandle::indexOfChild(PoolHandle t) const {
  return object()->indexOfChild(t.object());
}

bool PoolHandle::hasChild(PoolHandle t) const {
  return object()->hasChild(t.object());
}

PoolHandle PoolHandle::commonAncestorWith(PoolHandle t,
                                          bool includeTheseNodes) const {
  if (includeTheseNodes && *(const_cast<PoolHandle *>(this)) == t) {
    return t;
  }
  PoolHandle p = includeTheseNodes ? *this : parent();
  while (!p.isUninitialized()) {
    if (t.hasAncestor(p, includeTheseNodes)) {
      return p;
    }
    p = p.parent();
  }
  return PoolHandle();
}

PoolHandle PoolHandle::childAtIndex(int i) const {
  return PoolHandle(object()->childAtIndex(i));
}

void PoolHandle::replaceWithInPlace(PoolHandle t) {
  assert(!isUninitialized());
  PoolHandle p = parent();
  if (p.isUninitialized()) {
    t.detachFromParent();
  } else {
    p.replaceChildInPlace(*this, t);
  }
}

void PoolHandle::replaceChildInPlace(PoolHandle oldChild, PoolHandle newChild) {
  assert(!oldChild.isUninitialized());
  assert(!newChild.isUninitialized());
  assert(hasChild(oldChild));

  if (oldChild == newChild) {
    return;
  }
  assert(!oldChild.hasAncestor(newChild, true));

  assert(!isUninitialized());

  // If the new child has a parent, detach from it
  newChild.detachFromParent();

  // Move the new child
  assert(newChild.isGhost() || newChild.parent().isUninitialized());
  Pool::sharedPool->move(oldChild.object(), newChild.object(),
                         newChild.numberOfChildren());
  newChild.object()->retain();
  newChild.setParentIdentifier(identifier());

  // Move the old child
  Pool::sharedPool->move(Pool::sharedPool->last(), oldChild.object(),
                         oldChild.numberOfChildren());
  oldChild.object()->release(oldChild.numberOfChildren());
  oldChild.deleteParentIdentifier();
}

void PoolHandle::replaceChildAtIndexInPlace(int oldChildIndex,
                                            PoolHandle newChild) {
  assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
  PoolHandle oldChild = childAtIndex(oldChildIndex);
  replaceChildInPlace(oldChild, newChild);
}

void PoolHandle::replaceChildWithGhostInPlace(PoolHandle t) {
  Ghost ghost = Ghost::Builder();
  return replaceChildInPlace(t, ghost);
}

void PoolHandle::mergeChildrenAtIndexInPlace(PoolHandle t, int i) {
  /* mergeChildrenAtIndexInPlace should only be called with a tree that can
   * have any number of children, so there is no need to replace the stolen
   * children with ghosts. */
  // TODO assert this and t are "dynamic" trees
  assert(i >= 0 && i <= numberOfChildren());
  // Steal operands
  int numberOfNewChildren = t.numberOfChildren();
  if (i < numberOfChildren()) {
    Pool::sharedPool->moveChildren(object()->childAtIndex(i), t.object());
  } else {
    Pool::sharedPool->moveChildren(object()->lastDescendant()->next(),
                                   t.object());
  }
  object()->incrementNumberOfChildren(numberOfNewChildren);
  t.object()->eraseNumberOfChildren();
  for (int j = 0; j < numberOfNewChildren; j++) {
    assert(i + j < numberOfChildren());
    childAtIndex(i + j).setParentIdentifier(identifier());
  }
  // If t is a child, remove it
  if (object()->hasChild(t.object())) {
    removeChildInPlace(t, 0);
  }
}

void PoolHandle::swapChildrenInPlace(int i, int j) {
  assert(i >= 0 && i < numberOfChildren());
  assert(j >= 0 && j < numberOfChildren());
  if (i == j) {
    return;
  }
  int firstChildIndex = i < j ? i : j;
  int secondChildIndex = i > j ? i : j;
  PoolHandle firstChild = childAtIndex(firstChildIndex);
  PoolHandle secondChild = childAtIndex(secondChildIndex);
  Pool::sharedPool->move(firstChild.object()->nextSibling(),
                         secondChild.object(), secondChild.numberOfChildren());
  Pool::sharedPool->move(childAtIndex(secondChildIndex).object()->nextSibling(),
                         firstChild.object(), firstChild.numberOfChildren());
}

#if POINCARE_TREE_LOG
void PoolHandle::log() const {
  if (!isUninitialized()) {
    return object()->log();
  }
  std::cout << "\n<Uninitialized PoolHandle/>" << std::endl;
}
#endif

/* Protected */

// Add
void PoolHandle::addChildAtIndexInPlace(PoolHandle t, int index,
                                        int currentNumberOfChildren) {
  assert(!isUninitialized());
  assert(!t.isUninitialized());
  assert(index >= 0 && index <= currentNumberOfChildren);

  // If t has a parent, detach t from it.
  t.detachFromParent();
  assert(t.parent().isUninitialized());

  // Move t
  PoolObject *newChildPosition = object()->next();
  for (int i = 0; i < index; i++) {
    newChildPosition = newChildPosition->nextSibling();
  }
  Pool::sharedPool->move(newChildPosition, t.object(), t.numberOfChildren());
  t.object()->retain();
  object()->incrementNumberOfChildren();
  t.setParentIdentifier(identifier());

  object()->didChangeArity(currentNumberOfChildren + 1);
}

// Remove

void PoolHandle::removeChildAtIndexInPlace(int i) {
  assert(!isUninitialized());
  int nbOfChildren = numberOfChildren();
  assert(i >= 0 && i < nbOfChildren);
  PoolHandle t = childAtIndex(i);
  removeChildInPlace(t, t.numberOfChildren());

  object()->didChangeArity(nbOfChildren - 1);
}

void PoolHandle::removeChildInPlace(PoolHandle t, int childNumberOfChildren) {
  assert(!isUninitialized());
  Pool::sharedPool->move(Pool::sharedPool->last(), t.object(),
                         childNumberOfChildren);
  t.object()->release(childNumberOfChildren);
  t.deleteParentIdentifier();
  object()->incrementNumberOfChildren(-1);
}

/* Private */

void PoolHandle::detachFromParent() {
  PoolHandle myParent = parent();
  if (!myParent.isUninitialized()) {
    int idxInParent = myParent.indexOfChild(*this);
    myParent.replaceChildAtIndexWithGhostInPlace(idxInParent);
  }
  assert(parent().isUninitialized());
}

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

PoolHandle PoolHandle::Builder(PoolObject::Initializer initializer, size_t size,
                               int numberOfChildren) {
  void *bufferNode = Pool::sharedPool->alloc(size);
  PoolObject *node = initializer(bufferNode);
  node->setNumberOfChildren(numberOfChildren);
  return PoolHandle::BuildWithGhostChildren(node);
}

PoolHandle PoolHandle::BuilderWithChildren(PoolObject::Initializer initializer,
                                           size_t size, const Tuple &children) {
  PoolHandle h = Builder(initializer, size, children.size());
  size_t i = 0;
  for (PoolHandle child : children) {
    h.replaceChildAtIndexInPlace(i++, child);
  }
  return h;
}

template <class T, class U>
T PoolHandle::NAryBuilder(const Tuple &children) {
  PoolHandle h = Builder<U>();
  size_t i = 0;
  for (PoolHandle child : children) {
    h.addChildAtIndexInPlace(child, i, i);
    i++;
  }
  return static_cast<T &>(h);
}

template <class T, class U>
T PoolHandle::FixedArityBuilder(const Tuple &children) {
  PoolHandle h = Builder<U>();
  size_t i = 0;
  for (PoolHandle child : children) {
    h.replaceChildAtIndexInPlace(i++, child);
  }
  return static_cast<T &>(h);
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
