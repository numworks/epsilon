#include <poincare/old/ghost.h>
#include <poincare/old/poincare_expressions.h>
#include <poincare/old/pool_handle.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Clone */

PoolHandle PoolHandle::clone() const {
  assert(!isUninitialized());
  PoolObject *nodeCopy = Pool::sharedPool->deepCopy(node());
  nodeCopy->deleteParentIdentifier();
  return PoolHandle(nodeCopy);
}

/* Hierarchy operations */
PoolObject *PoolHandle::node() const {
  assert(hasNode(m_identifier));
  return Pool::sharedPool->node(m_identifier);
}

size_t PoolHandle::size() const {
  return node()->deepSize(node()->numberOfChildren());
}

PoolHandle PoolHandle::parent() const {
  return (isUninitialized() || node()->parent() == nullptr)
             ? PoolHandle()
             : PoolHandle(node()->parent());
}

int PoolHandle::indexOfChild(PoolHandle t) const {
  return node()->indexOfChild(t.node());
}

bool PoolHandle::hasChild(PoolHandle t) const {
  return node()->hasChild(t.node());
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
  return PoolHandle(node()->childAtIndex(i));
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
  Pool::sharedPool->move(oldChild.node(), newChild.node(),
                         newChild.numberOfChildren());
  newChild.node()->retain();
  newChild.setParentIdentifier(identifier());

  // Move the old child
  Pool::sharedPool->move(Pool::sharedPool->last(), oldChild.node(),
                         oldChild.numberOfChildren());
  oldChild.node()->release(oldChild.numberOfChildren());
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
    Pool::sharedPool->moveChildren(node()->childAtIndex(i), t.node());
  } else {
    Pool::sharedPool->moveChildren(node()->lastDescendant()->next(), t.node());
  }
  node()->incrementNumberOfChildren(numberOfNewChildren);
  t.node()->eraseNumberOfChildren();
  for (int j = 0; j < numberOfNewChildren; j++) {
    assert(i + j < numberOfChildren());
    childAtIndex(i + j).setParentIdentifier(identifier());
  }
  // If t is a child, remove it
  if (node()->hasChild(t.node())) {
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
  Pool::sharedPool->move(firstChild.node()->nextSibling(), secondChild.node(),
                         secondChild.numberOfChildren());
  Pool::sharedPool->move(childAtIndex(secondChildIndex).node()->nextSibling(),
                         firstChild.node(), firstChild.numberOfChildren());
}

#if POINCARE_TREE_LOG
void PoolHandle::log() const {
  if (!isUninitialized()) {
    return node()->log();
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
  PoolObject *newChildPosition = node()->next();
  for (int i = 0; i < index; i++) {
    newChildPosition = newChildPosition->nextSibling();
  }
  Pool::sharedPool->move(newChildPosition, t.node(), t.numberOfChildren());
  t.node()->retain();
  node()->incrementNumberOfChildren();
  t.setParentIdentifier(identifier());

  node()->didChangeArity(currentNumberOfChildren + 1);
}

// Remove

void PoolHandle::removeChildAtIndexInPlace(int i) {
  assert(!isUninitialized());
  int nbOfChildren = numberOfChildren();
  assert(i >= 0 && i < nbOfChildren);
  PoolHandle t = childAtIndex(i);
  removeChildInPlace(t, t.numberOfChildren());

  node()->didChangeArity(nbOfChildren - 1);
}

void PoolHandle::removeChildInPlace(PoolHandle t, int childNumberOfChildren) {
  assert(!isUninitialized());
  Pool::sharedPool->move(Pool::sharedPool->last(), t.node(),
                         childNumberOfChildren);
  t.node()->release(childNumberOfChildren);
  t.deleteParentIdentifier();
  node()->incrementNumberOfChildren(-1);
}

void PoolHandle::removeChildrenInPlace(int currentNumberOfChildren) {
  assert(!isUninitialized());
  deleteParentIdentifierInChildren();
  Pool::sharedPool->removeChildren(node(), currentNumberOfChildren);
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
               i * Helpers::AlignedSize(sizeof(GhostNode), ByteAlignment));
  }
  return PoolHandle(node);
}

void PoolHandle::setIdentifierAndRetain(uint16_t newId) {
  m_identifier = newId;
  if (!isUninitialized()) {
    node()->retain();
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

template Addition PoolHandle::NAryBuilder<Addition, AdditionNode>(
    const Tuple &);
template ComplexCartesian PoolHandle::FixedArityBuilder<
    ComplexCartesian, ComplexCartesianNode>(const Tuple &);
template Dependency PoolHandle::FixedArityBuilder<Dependency, DependencyNode>(
    const Tuple &);
template Derivative PoolHandle::FixedArityBuilder<Derivative, DerivativeNode>(
    const Tuple &);
template DistributionDispatcher PoolHandle::NAryBuilder<
    DistributionDispatcher, DistributionDispatcherNode>(const Tuple &);
template EmptyExpression PoolHandle::FixedArityBuilder<
    EmptyExpression, EmptyExpressionNode>(const Tuple &);
template FloatList<double> PoolHandle::NAryBuilder<FloatList<double>, ListNode>(
    const Tuple &);
template FloatList<float> PoolHandle::NAryBuilder<FloatList<float>, ListNode>(
    const Tuple &);
template Ghost PoolHandle::FixedArityBuilder<Ghost, GhostNode>(const Tuple &);
template GreatCommonDivisor PoolHandle::NAryBuilder<
    GreatCommonDivisor, GreatCommonDivisorNode>(const Tuple &);
template Integral PoolHandle::FixedArityBuilder<Integral, IntegralNode>(
    const Tuple &);
template LeastCommonMultiple PoolHandle::NAryBuilder<
    LeastCommonMultiple, LeastCommonMultipleNode>(const Tuple &);
template OList PoolHandle::NAryBuilder<OList, ListNode>(const Tuple &);
template ListComplex<double> PoolHandle::NAryBuilder<
    ListComplex<double>, ListComplexNode<double>>(const Tuple &);
template ListComplex<float> PoolHandle::NAryBuilder<
    ListComplex<float>, ListComplexNode<float>>(const Tuple &);
template ListElement
PoolHandle::FixedArityBuilder<ListElement, ListAccessNode<1>>(const Tuple &);
template ListSlice PoolHandle::FixedArityBuilder<ListSlice, ListAccessNode<2>>(
    const Tuple &);
template ListSequence
PoolHandle::FixedArityBuilder<ListSequence, ListSequenceNode>(const Tuple &);
template ListSort PoolHandle::FixedArityBuilder<ListSort, ListSortNode>(
    const Tuple &);
template OMatrix PoolHandle::NAryBuilder<OMatrix, MatrixNode>(const Tuple &);
template MatrixComplex<double> PoolHandle::NAryBuilder<
    MatrixComplex<double>, MatrixComplexNode<double>>(const Tuple &);
template MatrixComplex<float> PoolHandle::NAryBuilder<
    MatrixComplex<float>, MatrixComplexNode<float>>(const Tuple &);
template MixedFraction
PoolHandle::FixedArityBuilder<MixedFraction, MixedFractionNode>(const Tuple &);
template Multiplication
PoolHandle::NAryBuilder<Multiplication, MultiplicationNode>(const Tuple &);
template Opposite PoolHandle::FixedArityBuilder<Opposite, OppositeNode>(
    const Tuple &);
template Parenthesis
PoolHandle::FixedArityBuilder<Parenthesis, ParenthesisNode>(const Tuple &);
template PiecewiseOperator PoolHandle::NAryBuilder<
    PiecewiseOperator, PiecewiseOperatorNode>(const Tuple &);
template Product PoolHandle::FixedArityBuilder<Product, ProductNode>(
    const Tuple &);
template Subtraction
PoolHandle::FixedArityBuilder<Subtraction, SubtractionNode>(const Tuple &);
template Sum PoolHandle::FixedArityBuilder<Sum, SumNode>(const Tuple &);
template Undefined PoolHandle::FixedArityBuilder<Undefined, UndefinedNode>(
    const Tuple &);
template Nonreal PoolHandle::FixedArityBuilder<Nonreal, NonrealNode>(
    const Tuple &);

}  // namespace Poincare
