#include <poincare/tree_handle.h>
#include <poincare/ghost.h>
#include <poincare_expressions.h>
#include <poincare_layouts.h>

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

TreeHandle TreeHandle::commonAncestorWith(TreeHandle t) const {
  if (*(const_cast<TreeHandle *>(this)) == t) {
    return t;
  }
  TreeHandle p = *this;
  while (!p.isUninitialized()) {
    if (t.hasAncestor(p, true)) {
      return p;
    }
    p = p.parent();
  }
  return TreeHandle();
}

TreeHandle TreeHandle::childAtIndex(int i) const { return TreeHandle(node()->childAtIndex(i)); }

void TreeHandle::replaceWithInPlace(TreeHandle t) {
  assert(!isUninitialized());
  TreeHandle p = parent();
  if (p.isUninitialized()) {
    t.detachFromParent();
  } else {
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
  assert(!oldChild.hasAncestor(newChild, true));

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

  node()->didChangeArity(currentNumberOfChildren+1);
}

// Remove

void TreeHandle::removeChildAtIndexInPlace(int i) {
  assert(!isUninitialized());
  int nbOfChildren = numberOfChildren();
  assert(i >= 0 && i < nbOfChildren);
  TreeHandle t = childAtIndex(i);
  removeChildInPlace(t, t.numberOfChildren());

  node()->didChangeArity(nbOfChildren - 1);
}

void TreeHandle::removeChildInPlace(TreeHandle t, int childNumberOfChildren) {
  assert(!isUninitialized());
  TreePool::sharedPool()->move(TreePool::sharedPool()->last(), t.node(), childNumberOfChildren);
  t.node()->release(childNumberOfChildren);
  t.deleteParentIdentifier();
  node()->incrementNumberOfChildren(-1);
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
T TreeHandle::NAryBuilder(const Tuple & children) {
  TreeHandle h = Builder<U>();
  size_t i = 0;
  for (TreeHandle child : children) {
    h.addChildAtIndexInPlace(child, i, i);
    i++;
  }
  return static_cast<T &>(h);
}

template <class T, class U>
T TreeHandle::FixedArityBuilder(const Tuple & children) {
  TreeHandle h = Builder<U>();
  size_t i = 0;
  for (TreeHandle child : children) {
    h.replaceChildAtIndexInPlace(i++, child);
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

void TreeHandle::setIdentifierAndRetain(uint16_t newId) {
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

void TreeHandle::release(uint16_t identifier) {
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

template AbsoluteValue TreeHandle::FixedArityBuilder<AbsoluteValue, AbsoluteValueNode>(const Tuple &);
template AbsoluteValueLayout TreeHandle::FixedArityBuilder<AbsoluteValueLayout, AbsoluteValueLayoutNode>(const Tuple &);
template Addition TreeHandle::NAryBuilder<Addition, AdditionNode>(const Tuple &);
template ArcCosecant TreeHandle::FixedArityBuilder<ArcCosecant, ArcCosecantNode>(const Tuple &);
template ArcCosine TreeHandle::FixedArityBuilder<ArcCosine, ArcCosineNode>(const Tuple &);
template ArcCotangent TreeHandle::FixedArityBuilder<ArcCotangent, ArcCotangentNode>(const Tuple &);
template ArcSecant TreeHandle::FixedArityBuilder<ArcSecant, ArcSecantNode>(const Tuple &);
template ArcSine TreeHandle::FixedArityBuilder<ArcSine, ArcSineNode>(const Tuple &);
template ArcTangent TreeHandle::FixedArityBuilder<ArcTangent, ArcTangentNode>(const Tuple &);
template BinomialCoefficient TreeHandle::FixedArityBuilder<BinomialCoefficient, BinomialCoefficientNode>(const Tuple &);
template BinomialCoefficientLayout TreeHandle::FixedArityBuilder<BinomialCoefficientLayout, BinomialCoefficientLayoutNode>(const Tuple &);
template Ceiling TreeHandle::FixedArityBuilder<Ceiling, CeilingNode>(const Tuple &);
template CeilingLayout TreeHandle::FixedArityBuilder<CeilingLayout, CeilingLayoutNode>(const Tuple &);
template CommonLogarithm TreeHandle::FixedArityBuilder<CommonLogarithm, LogarithmNode<1> >(const Tuple &);
template ComplexArgument TreeHandle::FixedArityBuilder<ComplexArgument, ComplexArgumentNode>(const Tuple &);
template ComplexCartesian TreeHandle::FixedArityBuilder<ComplexCartesian, ComplexCartesianNode>(const Tuple &);
template CondensedSumLayout TreeHandle::FixedArityBuilder<CondensedSumLayout, CondensedSumLayoutNode>(const Tuple &);
template Conjugate TreeHandle::FixedArityBuilder<Conjugate, ConjugateNode>(const Tuple &);
template ConjugateLayout TreeHandle::FixedArityBuilder<ConjugateLayout, ConjugateLayoutNode>(const Tuple &);
template Cosecant TreeHandle::FixedArityBuilder<Cosecant, CosecantNode>(const Tuple &);
template Cotangent TreeHandle::FixedArityBuilder<Cotangent, CotangentNode>(const Tuple &);
template Cosine TreeHandle::FixedArityBuilder<Cosine, CosineNode>(const Tuple &);
template Dependency TreeHandle::FixedArityBuilder<Dependency, DependencyNode>(const Tuple &);
template Derivative TreeHandle::FixedArityBuilder<Derivative, DerivativeNode>(const Tuple &);
template DerivativeLayout TreeHandle::FixedArityBuilder<DerivativeLayout, DerivativeLayoutNode>(const Tuple &);
template Determinant TreeHandle::FixedArityBuilder<Determinant, DeterminantNode>(const Tuple &);
template Dimension TreeHandle::FixedArityBuilder<Dimension, DimensionNode>(const Tuple &);
template DistributionDispatcher TreeHandle::NAryBuilder<DistributionDispatcher, DistributionDispatcherNode>(const Tuple &);
template Division TreeHandle::FixedArityBuilder<Division, DivisionNode>(const Tuple &);
template DivisionQuotient TreeHandle::FixedArityBuilder<DivisionQuotient, DivisionQuotientNode>(const Tuple &);
template DivisionRemainder TreeHandle::FixedArityBuilder<DivisionRemainder, DivisionRemainderNode>(const Tuple &);
template EmptyExpression TreeHandle::FixedArityBuilder<EmptyExpression, EmptyExpressionNode>(const Tuple &);
template Equal TreeHandle::FixedArityBuilder<Equal, EqualNode>(const Tuple &);
template Factor TreeHandle::FixedArityBuilder<Factor, FactorNode>(const Tuple &);
template Factorial TreeHandle::FixedArityBuilder<Factorial, FactorialNode>(const Tuple &);
template FloatList<double> TreeHandle::NAryBuilder<FloatList<double>, ListNode>(const Tuple &);
template FloatList<float> TreeHandle::NAryBuilder<FloatList<float>, ListNode>(const Tuple &);
template Floor TreeHandle::FixedArityBuilder<Floor, FloorNode>(const Tuple &);
template FloorLayout TreeHandle::FixedArityBuilder<FloorLayout, FloorLayoutNode>(const Tuple &);
template FracPart TreeHandle::FixedArityBuilder<FracPart, FracPartNode>(const Tuple &);
template FractionLayout TreeHandle::FixedArityBuilder<FractionLayout, FractionLayoutNode>(const Tuple &);
template Ghost TreeHandle::FixedArityBuilder<Ghost, GhostNode>(const Tuple &);
template GreatCommonDivisor TreeHandle::NAryBuilder<GreatCommonDivisor, GreatCommonDivisorNode>(const Tuple &);
template HorizontalLayout TreeHandle::NAryBuilder<HorizontalLayout, HorizontalLayoutNode>(const Tuple &);
template HyperbolicArcCosine TreeHandle::FixedArityBuilder<HyperbolicArcCosine, HyperbolicArcCosineNode>(const Tuple &);
template HyperbolicArcSine TreeHandle::FixedArityBuilder<HyperbolicArcSine, HyperbolicArcSineNode>(const Tuple &);
template HyperbolicArcTangent TreeHandle::FixedArityBuilder<HyperbolicArcTangent, HyperbolicArcTangentNode>(const Tuple &);
template HyperbolicCosine TreeHandle::FixedArityBuilder<HyperbolicCosine, HyperbolicCosineNode>(const Tuple &);
template HyperbolicSine TreeHandle::FixedArityBuilder<HyperbolicSine, HyperbolicSineNode>(const Tuple &);
template HyperbolicTangent TreeHandle::FixedArityBuilder<HyperbolicTangent, HyperbolicTangentNode>(const Tuple &);
template Inferior TreeHandle::FixedArityBuilder<Inferior, InferiorNode>(const Tuple &);
template InferiorEqual TreeHandle::FixedArityBuilder<InferiorEqual, InferiorEqualNode>(const Tuple &);
template ImaginaryPart TreeHandle::FixedArityBuilder<ImaginaryPart, ImaginaryPartNode>(const Tuple &);
template Integral TreeHandle::FixedArityBuilder<Integral, IntegralNode>(const Tuple &);
template IntegralLayout TreeHandle::FixedArityBuilder<IntegralLayout, IntegralLayoutNode>(const Tuple &);
template LeastCommonMultiple TreeHandle::NAryBuilder<LeastCommonMultiple, LeastCommonMultipleNode>(const Tuple &);
template LeftCurlyBraceLayout TreeHandle::FixedArityBuilder<LeftCurlyBraceLayout, LeftCurlyBraceLayoutNode>(const Tuple &);
template LeftParenthesisLayout TreeHandle::FixedArityBuilder<LeftParenthesisLayout, LeftParenthesisLayoutNode>(const Tuple &);
template LeftSquareBracketLayout TreeHandle::FixedArityBuilder<LeftSquareBracketLayout, LeftSquareBracketLayoutNode>(const Tuple &);
template LetterAWithSubAndSuperscriptLayout TreeHandle::FixedArityBuilder<LetterAWithSubAndSuperscriptLayout, LetterAWithSubAndSuperscriptLayoutNode>(const Tuple &);
template LetterCWithSubAndSuperscriptLayout TreeHandle::FixedArityBuilder<LetterCWithSubAndSuperscriptLayout, LetterCWithSubAndSuperscriptLayoutNode>(const Tuple &);
template List TreeHandle::NAryBuilder<List, ListNode>(const Tuple &);
template ListComplex<double> TreeHandle::NAryBuilder<ListComplex<double>, ListComplexNode<double> >(const Tuple &);
template ListComplex<float> TreeHandle::NAryBuilder<ListComplex<float>, ListComplexNode<float> >(const Tuple &);
template ListElement TreeHandle::FixedArityBuilder<ListElement, ListAccessNode<1>>(const Tuple &);
template ListMaximum TreeHandle::FixedArityBuilder<ListMaximum, ListMaximumNode>(const Tuple &);
template ListMean TreeHandle::FixedArityBuilder<ListMean, ListMeanNode<1>>(const Tuple &);
template ListMean TreeHandle::FixedArityBuilder<ListMean, ListMeanNode<2>>(const Tuple &);
template ListMedian TreeHandle::FixedArityBuilder<ListMedian, ListMedianNode<1>>(const Tuple &);
template ListMedian TreeHandle::FixedArityBuilder<ListMedian, ListMedianNode<2>>(const Tuple &);
template ListMinimum TreeHandle::FixedArityBuilder<ListMinimum, ListMinimumNode>(const Tuple &);
template ListProduct TreeHandle::FixedArityBuilder<ListProduct, ListProductNode>(const Tuple &);
template ListSampleStandardDeviation TreeHandle::FixedArityBuilder<ListSampleStandardDeviation, ListSampleStandardDeviationNode<1>>(const Tuple &);
template ListSampleStandardDeviation TreeHandle::FixedArityBuilder<ListSampleStandardDeviation, ListSampleStandardDeviationNode<2>>(const Tuple &);
template ListSlice TreeHandle::FixedArityBuilder<ListSlice, ListAccessNode<2>>(const Tuple &);
template ListSequence TreeHandle::FixedArityBuilder<ListSequence, ListSequenceNode>(const Tuple &);
template ListSequenceLayout TreeHandle::FixedArityBuilder<ListSequenceLayout, ListSequenceLayoutNode>(const Tuple &);
template ListSort TreeHandle::FixedArityBuilder<ListSort, ListSortNode>(const Tuple &);
template ListStandardDeviation TreeHandle::FixedArityBuilder<ListStandardDeviation, ListStandardDeviationNode<1>>(const Tuple &);
template ListStandardDeviation TreeHandle::FixedArityBuilder<ListStandardDeviation, ListStandardDeviationNode<2>>(const Tuple &);
template ListSum TreeHandle::FixedArityBuilder<ListSum, ListSumNode>(const Tuple &);
template ListVariance TreeHandle::FixedArityBuilder<ListVariance, ListVarianceNode<1>>(const Tuple &);
template ListVariance TreeHandle::FixedArityBuilder<ListVariance, ListVarianceNode<2>>(const Tuple &);
template Logarithm TreeHandle::FixedArityBuilder<Logarithm, LogarithmNode<2> >(const Tuple &);
template Matrix TreeHandle::NAryBuilder<Matrix, MatrixNode>(const Tuple &);
template MatrixComplex<double> TreeHandle::NAryBuilder<MatrixComplex<double>, MatrixComplexNode<double> >(const Tuple &);
template MatrixComplex<float> TreeHandle::NAryBuilder<MatrixComplex<float>, MatrixComplexNode<float> >(const Tuple &);
template MatrixIdentity TreeHandle::FixedArityBuilder<MatrixIdentity, MatrixIdentityNode>(const Tuple &);
template MatrixInverse TreeHandle::FixedArityBuilder<MatrixInverse, MatrixInverseNode>(const Tuple &);
template MatrixTrace TreeHandle::FixedArityBuilder<MatrixTrace, MatrixTraceNode>(const Tuple &);
template MatrixTranspose TreeHandle::FixedArityBuilder<MatrixTranspose, MatrixTransposeNode>(const Tuple &);
template MatrixRowEchelonForm TreeHandle::FixedArityBuilder<MatrixRowEchelonForm, MatrixRowEchelonFormNode>(const Tuple &);
template MatrixReducedRowEchelonForm TreeHandle::FixedArityBuilder<MatrixReducedRowEchelonForm, MatrixReducedRowEchelonFormNode>(const Tuple &);
template Multiplication TreeHandle::NAryBuilder<Multiplication, MultiplicationNode>(const Tuple &);
template NaperianLogarithm TreeHandle::FixedArityBuilder<NaperianLogarithm, NaperianLogarithmNode>(const Tuple &);
template NthRoot TreeHandle::FixedArityBuilder<NthRoot, NthRootNode>(const Tuple &);
template Opposite TreeHandle::FixedArityBuilder<Opposite, OppositeNode>(const Tuple &);
template Parenthesis TreeHandle::FixedArityBuilder<Parenthesis, ParenthesisNode>(const Tuple &);
template PermuteCoefficient TreeHandle::FixedArityBuilder<PermuteCoefficient, PermuteCoefficientNode>(const Tuple &);
template Power TreeHandle::FixedArityBuilder<Power, PowerNode>(const Tuple &);
template Product TreeHandle::FixedArityBuilder<Product, ProductNode>(const Tuple &);
template ProductLayout TreeHandle::FixedArityBuilder<ProductLayout, ProductLayoutNode>(const Tuple &);
template Randint TreeHandle::FixedArityBuilder<Randint, RandintNode>(const Tuple &);
template Random TreeHandle::FixedArityBuilder<Random, RandomNode>(const Tuple &);
template RealPart TreeHandle::FixedArityBuilder<RealPart, RealPartNode>(const Tuple &);
template RightCurlyBraceLayout TreeHandle::FixedArityBuilder<RightCurlyBraceLayout, RightCurlyBraceLayoutNode>(const Tuple &);
template RightParenthesisLayout TreeHandle::FixedArityBuilder<RightParenthesisLayout, RightParenthesisLayoutNode>(const Tuple &);
template RightSquareBracketLayout TreeHandle::FixedArityBuilder<RightSquareBracketLayout, RightSquareBracketLayoutNode>(const Tuple &);
template Round TreeHandle::FixedArityBuilder<Round, RoundNode>(const Tuple &);
template Secant TreeHandle::FixedArityBuilder<Secant, SecantNode>(const Tuple &);
template SignFunction TreeHandle::FixedArityBuilder<SignFunction, SignFunctionNode>(const Tuple &);
template Sine TreeHandle::FixedArityBuilder<Sine, SineNode>(const Tuple &);
template SquareRoot TreeHandle::FixedArityBuilder<SquareRoot, SquareRootNode>(const Tuple &);
template Store TreeHandle::FixedArityBuilder<Store, StoreNode>(const Tuple &);
template Subtraction TreeHandle::FixedArityBuilder<Subtraction, SubtractionNode>(const Tuple &);
template Superior TreeHandle::FixedArityBuilder<Superior, SuperiorNode>(const Tuple &);
template SuperiorEqual TreeHandle::FixedArityBuilder<SuperiorEqual, SuperiorEqualNode>(const Tuple &);
template Sum TreeHandle::FixedArityBuilder<Sum, SumNode>(const Tuple &);
template SumLayout TreeHandle::FixedArityBuilder<SumLayout, SumLayoutNode>(const Tuple &);
template Tangent TreeHandle::FixedArityBuilder<Tangent, TangentNode>(const Tuple &);
template Undefined TreeHandle::FixedArityBuilder<Undefined, UndefinedNode>(const Tuple &);
template UnitConvert TreeHandle::FixedArityBuilder<UnitConvert, UnitConvertNode>(const Tuple &);
template Nonreal TreeHandle::FixedArityBuilder<Nonreal, NonrealNode>(const Tuple &);
template VectorCross TreeHandle::FixedArityBuilder<VectorCross, VectorCrossNode>(const Tuple &);
template VectorDot TreeHandle::FixedArityBuilder<VectorDot, VectorDotNode>(const Tuple &);
template VectorNorm TreeHandle::FixedArityBuilder<VectorNorm, VectorNormNode>(const Tuple &);
template VectorNormLayout TreeHandle::FixedArityBuilder<VectorNormLayout, VectorNormLayoutNode>(const Tuple &);
template MatrixLayout TreeHandle::NAryBuilder<MatrixLayout, MatrixLayoutNode>(const Tuple &);

}
