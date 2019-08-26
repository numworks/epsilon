#include <poincare/tree_handle.h>
#include <poincare_nodes.h>
#include <poincare_layouts.h>
#include <poincare/ghost.h>
#if POINCARE_TREE_LOG
#include <iostream>
#endif

namespace Poincare {

/* Clone */

TreeHandle TreeHandle::clone() const {
  assert(!isUninitialized());
  TreeNode * nodeCopy = TreePool::sharedPool()->deepCopy(node());
  nodeCopy->deleteParentIdentifier();
  return TreeHandle(nodeCopy);
}

/* Hierarchy operations */
TreeNode * TreeHandle::node() const { assert(hasNode(m_identifier)); return TreePool::sharedPool()->node(m_identifier); }

size_t TreeHandle::size() const { return node()->deepSize(node()->numberOfChildren()); }

TreeHandle TreeHandle::parent() const { return (isUninitialized() || node()->parent() == nullptr) ? TreeHandle() : TreeHandle(node()->parent()); }

int TreeHandle::indexOfChild(TreeHandle t) const { return node()->indexOfChild(t.node()); }

bool TreeHandle::hasChild(TreeHandle t) const { return node()->hasChild(t.node()); }

TreeHandle TreeHandle::childAtIndex(int i) const { return TreeHandle(node()->childAtIndex(i)); }

void TreeHandle::replaceWithInPlace(TreeHandle t) {
  assert(!isUninitialized());
  TreeHandle p = parent();
  if (!p.isUninitialized()) {
    p.replaceChildInPlace(*this, t);
  }
}

void TreeHandle::replaceChildInPlace(TreeHandle oldChild, TreeHandle newChild) {
  assert(!oldChild.isUninitialized());
  assert(!newChild.isUninitialized());
  assert(hasChild(oldChild));

  if (oldChild == newChild) {
    return;
  }

  assert(!isUninitialized());

  // If the new child has a parent, detach from it
  newChild.detachFromParent();

  // Move the new child
  assert(newChild.isGhost() || newChild.parent().isUninitialized());
  TreePool::sharedPool()->move(oldChild.node(), newChild.node(), newChild.numberOfChildren());
  newChild.node()->retain();
  newChild.setParentIdentifier(identifier());

  // Move the old child
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), oldChild.node(), oldChild.numberOfChildren());
  oldChild.node()->release(oldChild.numberOfChildren());
  oldChild.deleteParentIdentifier();
}

void TreeHandle::replaceChildAtIndexInPlace(int oldChildIndex, TreeHandle newChild) {
  assert(oldChildIndex >= 0 && oldChildIndex < numberOfChildren());
  TreeHandle oldChild = childAtIndex(oldChildIndex);
  replaceChildInPlace(oldChild, newChild);
}

void TreeHandle::replaceChildWithGhostInPlace(TreeHandle t) {
  Ghost ghost = Ghost::Builder();
  return replaceChildInPlace(t, ghost);
}

void TreeHandle::mergeChildrenAtIndexInPlace(TreeHandle t, int i) {
  /* mergeChildrenAtIndexInPlace should only be called with a tree that can
   * have any number of children, so there is no need to replace the stolen
   * children with ghosts. */
  // TODO assert this and t are "dynamic" trees
  assert(i >= 0 && i <= numberOfChildren());
  // Steal operands
  int numberOfNewChildren = t.numberOfChildren();
  if (i < numberOfChildren()) {
    TreePool::sharedPool()->moveChildren(node()->childAtIndex(i), t.node());
  } else {
    TreePool::sharedPool()->moveChildren(node()->lastDescendant()->next(), t.node());
  }
  node()->incrementNumberOfChildren(numberOfNewChildren);
  t.node()->eraseNumberOfChildren();
  for (int j = 0; j < numberOfNewChildren; j++) {
    assert(i+j < numberOfChildren());
    childAtIndex(i+j).setParentIdentifier(identifier());
  }
  // If t is a child, remove it
  if (node()->hasChild(t.node())) {
    removeChildInPlace(t, 0);
  }
}

void TreeHandle::swapChildrenInPlace(int i, int j) {
  assert(i >= 0 && i < numberOfChildren());
  assert(j >= 0 && j < numberOfChildren());
  if (i == j) {
    return;
  }
  int firstChildIndex = i < j ? i : j;
  int secondChildIndex = i > j ? i : j;
  TreeHandle firstChild = childAtIndex(firstChildIndex);
  TreeHandle secondChild = childAtIndex(secondChildIndex);
  TreePool::sharedPool()->move(firstChild.node()->nextSibling(), secondChild.node(), secondChild.numberOfChildren());
  TreePool::sharedPool()->move(childAtIndex(secondChildIndex).node()->nextSibling(), firstChild.node(), firstChild.numberOfChildren());
}

#if POINCARE_TREE_LOG
void TreeHandle::log() const {
  node()->log(std::cout);
  std::cout << std::endl;
}
#endif

/* Protected */

// Add
void TreeHandle::addChildAtIndexInPlace(TreeHandle t, int index, int currentNumberOfChildren) {
  assert(!isUninitialized());
  assert(!t.isUninitialized());
  assert(index >= 0 && index <= currentNumberOfChildren);

  // If t has a parent, detach t from it.
  t.detachFromParent();
  assert(t.parent().isUninitialized());

  // Move t
  TreeNode * newChildPosition = node()->next();
  for (int i = 0; i < index; i++) {
    newChildPosition = newChildPosition->nextSibling();
  }
  TreePool::sharedPool()->move(newChildPosition, t.node(), t.numberOfChildren());
  t.node()->retain();
  node()->incrementNumberOfChildren();
  t.setParentIdentifier(identifier());

  node()->didAddChildAtIndex(currentNumberOfChildren+1);
}

// Remove

void TreeHandle::removeChildAtIndexInPlace(int i) {
  assert(!isUninitialized());
  assert(i >= 0 && i < numberOfChildren());
  TreeHandle t = childAtIndex(i);
  removeChildInPlace(t, t.numberOfChildren());
}

void TreeHandle::removeChildInPlace(TreeHandle t, int childNumberOfChildren) {
  assert(!isUninitialized());
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node(), childNumberOfChildren);
  t.node()->release(childNumberOfChildren);
  t.deleteParentIdentifier();
  node()->decrementNumberOfChildren();
}

void TreeHandle::removeChildrenInPlace(int currentNumberOfChildren) {
  assert(!isUninitialized());
  deleteParentIdentifierInChildren();
  TreePool::sharedPool()->removeChildren(node(), currentNumberOfChildren);
}

/* Private */

void TreeHandle::detachFromParent() {
  TreeHandle myParent = parent();
  if (!myParent.isUninitialized()) {
    int idxInParent = myParent.indexOfChild(*this);
    myParent.replaceChildAtIndexWithGhostInPlace(idxInParent);
  }
  assert(parent().isUninitialized());
}


TreeHandle::TreeHandle(const TreeNode * node) : TreeHandle() {
  if (node != nullptr) {
    setIdentifierAndRetain(node->identifier());
  }
}

template <class U>
TreeHandle TreeHandle::Builder() {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(U));
  U * node = new (bufferNode) U();
  return TreeHandle::BuildWithGhostChildren(node);
}

template <class T, class U>
T TreeHandle::NAryBuilder(TreeHandle * children, size_t numberOfChildren) {
  TreeHandle h = Builder<U>();
  for (size_t i = 0; i < numberOfChildren; i++) {
    h.addChildAtIndexInPlace(children[i], i, i);
  }
  return static_cast<T &>(h);
}

template <class T, class U>
T TreeHandle::FixedArityBuilder(TreeHandle * children, size_t numberOfChildren) {
  TreeHandle h = Builder<U>();
  for (size_t i = 0; i < numberOfChildren; i++) {
    h.replaceChildAtIndexInPlace(i, children[i]);
  }
  return static_cast<T &>(h);
}

TreeHandle TreeHandle::BuildWithGhostChildren(TreeNode * node) {
  assert(node != nullptr);
  TreePool * pool = TreePool::sharedPool();
  int expectedNumberOfChildren = node->numberOfChildren();
  /* Ensure the pool is syntaxically correct by creating ghost children for
   * nodes that have a fixed, non-zero number of children. */
  for (int i = 0; i < expectedNumberOfChildren; i++) {
    GhostNode * ghost = new (pool->alloc(sizeof(GhostNode))) GhostNode();
    ghost->rename(pool->generateIdentifier(), false);
    ghost->retain();
    assert((char *)ghost == (char *)node->next() + i*Helpers::AlignedSize(sizeof(GhostNode), ByteAlignment));
  }
  node->rename(pool->generateIdentifier(), false);
  return TreeHandle(node);
}

void TreeHandle::setIdentifierAndRetain(int newId) {
  m_identifier = newId;
  if (!isUninitialized()) {
    node()->retain();
  }
}

void TreeHandle::setTo(const TreeHandle & tr) {
  /* We cannot use (*this)==tr because tr would need to be casted to
   * TreeHandle, which calls setTo and triggers an infinite loop */
  if (identifier() == tr.identifier()) {
    return;
  }
  int currentId = identifier();
  setIdentifierAndRetain(tr.identifier());
  release(currentId);
}

void TreeHandle::release(int identifier) {
  if (!hasNode(identifier)) {
    return;
  }
  TreeNode * node = TreePool::sharedPool()->node(identifier);
  if (node == nullptr) {
    /* The identifier is valid, but not the node: there must have been an
     * exception that deleted the pool. */
    return;
  }
  assert(node->identifier() == identifier);
  node->release(node->numberOfChildren());
}

template AbsoluteValue TreeHandle::FixedArityBuilder<AbsoluteValue, AbsoluteValueNode>(TreeHandle*, size_t);
template AbsoluteValueLayout TreeHandle::FixedArityBuilder<AbsoluteValueLayout, AbsoluteValueLayoutNode>(TreeHandle*, size_t);
template Addition TreeHandle::NAryBuilder<Addition, AdditionNode>(TreeHandle*, size_t);
template ArcCosine TreeHandle::FixedArityBuilder<ArcCosine, ArcCosineNode>(TreeHandle*, size_t);
template ArcSine TreeHandle::FixedArityBuilder<ArcSine, ArcSineNode>(TreeHandle*, size_t);
template ArcTangent TreeHandle::FixedArityBuilder<ArcTangent, ArcTangentNode>(TreeHandle*, size_t);
template BinomCDF TreeHandle::FixedArityBuilder<BinomCDF, BinomCDFNode>(TreeHandle*, size_t);
template BinomialCoefficient TreeHandle::FixedArityBuilder<BinomialCoefficient, BinomialCoefficientNode>(TreeHandle*, size_t);
template BinomialCoefficientLayout TreeHandle::FixedArityBuilder<BinomialCoefficientLayout, BinomialCoefficientLayoutNode>(TreeHandle*, size_t);
template BinomPDF TreeHandle::FixedArityBuilder<BinomPDF, BinomPDFNode>(TreeHandle*, size_t);
template Ceiling TreeHandle::FixedArityBuilder<Ceiling, CeilingNode>(TreeHandle*, size_t);
template CeilingLayout TreeHandle::FixedArityBuilder<CeilingLayout, CeilingLayoutNode>(TreeHandle*, size_t);
template CommonLogarithm TreeHandle::FixedArityBuilder<CommonLogarithm, LogarithmNode<1> >(TreeHandle*, size_t);
template ComplexArgument TreeHandle::FixedArityBuilder<ComplexArgument, ComplexArgumentNode>(TreeHandle*, size_t);
template ComplexCartesian TreeHandle::FixedArityBuilder<ComplexCartesian, ComplexCartesianNode>(TreeHandle*, size_t);
template CondensedSumLayout TreeHandle::FixedArityBuilder<CondensedSumLayout, CondensedSumLayoutNode>(TreeHandle*, size_t);
template ConfidenceInterval TreeHandle::FixedArityBuilder<ConfidenceInterval, ConfidenceIntervalNode>(TreeHandle*, size_t);
template Conjugate TreeHandle::FixedArityBuilder<Conjugate, ConjugateNode>(TreeHandle*, size_t);
template ConjugateLayout TreeHandle::FixedArityBuilder<ConjugateLayout, ConjugateLayoutNode>(TreeHandle*, size_t);
template Cosine TreeHandle::FixedArityBuilder<Cosine, CosineNode>(TreeHandle*, size_t);
template Derivative TreeHandle::FixedArityBuilder<Derivative, DerivativeNode>(TreeHandle*, size_t);
template Determinant TreeHandle::FixedArityBuilder<Determinant, DeterminantNode>(TreeHandle*, size_t);
template Division TreeHandle::FixedArityBuilder<Division, DivisionNode>(TreeHandle*, size_t);
template DivisionQuotient TreeHandle::FixedArityBuilder<DivisionQuotient, DivisionQuotientNode>(TreeHandle*, size_t);
template DivisionRemainder TreeHandle::FixedArityBuilder<DivisionRemainder, DivisionRemainderNode>(TreeHandle*, size_t);
template EmptyExpression TreeHandle::FixedArityBuilder<EmptyExpression, EmptyExpressionNode>(TreeHandle*, size_t);
template Equal TreeHandle::FixedArityBuilder<Equal, EqualNode>(TreeHandle*, size_t);
template Factor TreeHandle::FixedArityBuilder<Factor, FactorNode>(TreeHandle*, size_t);
template Factorial TreeHandle::FixedArityBuilder<Factorial, FactorialNode>(TreeHandle*, size_t);
template Floor TreeHandle::FixedArityBuilder<Floor, FloorNode>(TreeHandle*, size_t);
template FloorLayout TreeHandle::FixedArityBuilder<FloorLayout, FloorLayoutNode>(TreeHandle*, size_t);
template FracPart TreeHandle::FixedArityBuilder<FracPart, FracPartNode>(TreeHandle*, size_t);
template FractionLayout TreeHandle::FixedArityBuilder<FractionLayout, FractionLayoutNode>(TreeHandle*, size_t);
template GreatCommonDivisor TreeHandle::FixedArityBuilder<GreatCommonDivisor, GreatCommonDivisorNode>(TreeHandle*, size_t);
template HorizontalLayout TreeHandle::NAryBuilder<HorizontalLayout, HorizontalLayoutNode>(TreeHandle*, size_t);
template HyperbolicArcCosine TreeHandle::FixedArityBuilder<HyperbolicArcCosine, HyperbolicArcCosineNode>(TreeHandle*, size_t);
template HyperbolicArcSine TreeHandle::FixedArityBuilder<HyperbolicArcSine, HyperbolicArcSineNode>(TreeHandle*, size_t);
template HyperbolicArcTangent TreeHandle::FixedArityBuilder<HyperbolicArcTangent, HyperbolicArcTangentNode>(TreeHandle*, size_t);
template HyperbolicCosine TreeHandle::FixedArityBuilder<HyperbolicCosine, HyperbolicCosineNode>(TreeHandle*, size_t);
template HyperbolicSine TreeHandle::FixedArityBuilder<HyperbolicSine, HyperbolicSineNode>(TreeHandle*, size_t);
template HyperbolicTangent TreeHandle::FixedArityBuilder<HyperbolicTangent, HyperbolicTangentNode>(TreeHandle*, size_t);
template ImaginaryPart TreeHandle::FixedArityBuilder<ImaginaryPart, ImaginaryPartNode>(TreeHandle*, size_t);
template Integral TreeHandle::FixedArityBuilder<Integral, IntegralNode>(TreeHandle*, size_t);
template IntegralLayout TreeHandle::FixedArityBuilder<IntegralLayout, IntegralLayoutNode>(TreeHandle*, size_t);
template InvBinom TreeHandle::FixedArityBuilder<InvBinom, InvBinomNode>(TreeHandle*, size_t);
template InvNorm TreeHandle::FixedArityBuilder<InvNorm, InvNormNode>(TreeHandle*, size_t);
template LeastCommonMultiple TreeHandle::FixedArityBuilder<LeastCommonMultiple, LeastCommonMultipleNode>(TreeHandle*, size_t);
template LeftParenthesisLayout TreeHandle::FixedArityBuilder<LeftParenthesisLayout, LeftParenthesisLayoutNode>(TreeHandle*, size_t);
template LeftSquareBracketLayout TreeHandle::FixedArityBuilder<LeftSquareBracketLayout, LeftSquareBracketLayoutNode>(TreeHandle*, size_t);
template Logarithm TreeHandle::FixedArityBuilder<Logarithm, LogarithmNode<2> >(TreeHandle*, size_t);
template Matrix TreeHandle::NAryBuilder<Matrix, MatrixNode>(TreeHandle*, size_t);
template MatrixComplex<double> TreeHandle::NAryBuilder<MatrixComplex<double>, MatrixComplexNode<double> >(TreeHandle*, size_t);
template MatrixComplex<float> TreeHandle::NAryBuilder<MatrixComplex<float>, MatrixComplexNode<float> >(TreeHandle*, size_t);
template MatrixDimension TreeHandle::FixedArityBuilder<MatrixDimension, MatrixDimensionNode>(TreeHandle*, size_t);
template MatrixIdentity TreeHandle::FixedArityBuilder<MatrixIdentity, MatrixIdentityNode>(TreeHandle*, size_t);
template MatrixInverse TreeHandle::FixedArityBuilder<MatrixInverse, MatrixInverseNode>(TreeHandle*, size_t);
template MatrixTrace TreeHandle::FixedArityBuilder<MatrixTrace, MatrixTraceNode>(TreeHandle*, size_t);
template MatrixTranspose TreeHandle::FixedArityBuilder<MatrixTranspose, MatrixTransposeNode>(TreeHandle*, size_t);
template Multiplication TreeHandle::NAryBuilder<Multiplication, MultiplicationNode>(TreeHandle*, size_t);
template NaperianLogarithm TreeHandle::FixedArityBuilder<NaperianLogarithm, NaperianLogarithmNode>(TreeHandle*, size_t);
template NormCDF TreeHandle::FixedArityBuilder<NormCDF, NormCDFNode>(TreeHandle*, size_t);
template NormCDF2 TreeHandle::FixedArityBuilder<NormCDF2, NormCDF2Node>(TreeHandle*, size_t);
template NormPDF TreeHandle::FixedArityBuilder<NormPDF, NormPDFNode>(TreeHandle*, size_t);
template NthRoot TreeHandle::FixedArityBuilder<NthRoot, NthRootNode>(TreeHandle*, size_t);
template Opposite TreeHandle::FixedArityBuilder<Opposite, OppositeNode>(TreeHandle*, size_t);
template Parenthesis TreeHandle::FixedArityBuilder<Parenthesis, ParenthesisNode>(TreeHandle*, size_t);
template PermuteCoefficient TreeHandle::FixedArityBuilder<PermuteCoefficient, PermuteCoefficientNode>(TreeHandle*, size_t);
template Power TreeHandle::FixedArityBuilder<Power, PowerNode>(TreeHandle*, size_t);
template PredictionInterval TreeHandle::FixedArityBuilder<PredictionInterval, PredictionIntervalNode>(TreeHandle*, size_t);
template Product TreeHandle::FixedArityBuilder<Product, ProductNode>(TreeHandle*, size_t);
template ProductLayout TreeHandle::FixedArityBuilder<ProductLayout, ProductLayoutNode>(TreeHandle*, size_t);
template Randint TreeHandle::FixedArityBuilder<Randint, RandintNode>(TreeHandle*, size_t);
template Random TreeHandle::FixedArityBuilder<Random, RandomNode>(TreeHandle*, size_t);
template RealPart TreeHandle::FixedArityBuilder<RealPart, RealPartNode>(TreeHandle*, size_t);
template RightParenthesisLayout TreeHandle::FixedArityBuilder<RightParenthesisLayout, RightParenthesisLayoutNode>(TreeHandle*, size_t);
template RightSquareBracketLayout TreeHandle::FixedArityBuilder<RightSquareBracketLayout, RightSquareBracketLayoutNode>(TreeHandle*, size_t);
template Round TreeHandle::FixedArityBuilder<Round, RoundNode>(TreeHandle*, size_t);
template SignFunction TreeHandle::FixedArityBuilder<SignFunction, SignFunctionNode>(TreeHandle*, size_t);
template SimplePredictionInterval TreeHandle::FixedArityBuilder<SimplePredictionInterval, SimplePredictionIntervalNode>(TreeHandle*, size_t);
template Sine TreeHandle::FixedArityBuilder<Sine, SineNode>(TreeHandle*, size_t);
template SquareRoot TreeHandle::FixedArityBuilder<SquareRoot, SquareRootNode>(TreeHandle*, size_t);
template Store TreeHandle::FixedArityBuilder<Store, StoreNode>(TreeHandle*, size_t);
template Subtraction TreeHandle::FixedArityBuilder<Subtraction, SubtractionNode>(TreeHandle*, size_t);
template Sum TreeHandle::FixedArityBuilder<Sum, SumNode>(TreeHandle*, size_t);
template SumLayout TreeHandle::FixedArityBuilder<SumLayout, SumLayoutNode>(TreeHandle*, size_t);
template Tangent TreeHandle::FixedArityBuilder<Tangent, TangentNode>(TreeHandle*, size_t);
template Undefined TreeHandle::FixedArityBuilder<Undefined, UndefinedNode>(TreeHandle*, size_t);
template Unreal TreeHandle::FixedArityBuilder<Unreal, UnrealNode>(TreeHandle*, size_t);
template MatrixLayout TreeHandle::NAryBuilder<MatrixLayout, MatrixLayoutNode>(TreeHandle*, size_t);

}
