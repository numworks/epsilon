#include <poincare/tree_pool.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/helpers.h>
#include <poincare/tree_handle.h>
#include <poincare/test/tree/blob_node.h>
#include <poincare/test/tree/pair_node.h>
#include <string.h>
#include <stdint.h>
#include <poincare_layouts.h>
#include <poincare_nodes.h>

namespace Poincare {

TreePool * TreePool::SharedStaticPool;

void TreePool::freeIdentifier(int identifier) {
  if (identifier >= 0 && identifier < MaxNumberOfNodes) {
    m_nodeForIdentifier[identifier] = nullptr;
    m_identifiers.push(identifier);
  }
}

template <typename T>
T * TreePool::createTreeNode(size_t size) {
  T * node = new(alloc(size)) T();
  if (size != sizeof(T)) {
    /* If the node does not have a standard size, it should init itself so that
     * T::size gives the right result, otherwise TreeNode::next() does not work
     * and addGhostChildrenAndRename might not work. */
    node->initToMatchSize(size);
  }
  addGhostChildrenAndRename(node);
  return node;
}

void TreePool::move(TreeNode * destination, TreeNode * source, int realNumberOfSourceChildren) {
  size_t moveSize = source->deepSize(realNumberOfSourceChildren);
  moveNodes(destination, source, moveSize);
}

void TreePool::moveChildren(TreeNode * destination, TreeNode * sourceParent) {
  size_t moveSize = sourceParent->deepSize(-1) - Helpers::AlignedSize(sourceParent->size(), ByteAlignment);
  moveNodes(destination, sourceParent->next(), moveSize);
}

void TreePool::removeChildren(TreeNode * node, int nodeNumberOfChildren) {
  for (int i = 0; i < nodeNumberOfChildren; i++) {
    TreeNode * child = node->childAtIndex(0);
    int childNumberOfChildren = child->numberOfChildren();
    /* The new child will be put at the address last(), but removed from its
     * previous position, hence the newAddress we use. */
    TreeNode * newAddress = (TreeNode *)((char *)last() - (char *)child->deepSize(childNumberOfChildren));
    move(last(), child, childNumberOfChildren);
    newAddress->release(newAddress->numberOfChildren());
  }
  node->eraseNumberOfChildren();
}

TreeNode * TreePool::deepCopy(TreeNode * node) {
  size_t size = node->deepSize(-1);
  return copyTreeFromAddress(static_cast<void *>(node), size);
}

TreeNode * TreePool::copyTreeFromAddress(const void * address, size_t size) {
  void * ptr = alloc(size);
  memcpy(ptr, address, size);
  TreeNode * copy = reinterpret_cast<TreeNode *>(ptr);
  renameNode(copy, false);
  for (TreeNode * child : copy->depthFirstChildren()) {
    renameNode(child, false);
    child->retain();
  }
  return copy;
}

void TreePool::removeChildrenAndDestroy(TreeNode * nodeToDestroy, int nodeNumberOfChildren) {
  removeChildren(nodeToDestroy, nodeNumberOfChildren);
  discardTreeNode(nodeToDestroy);
}

void TreePool::moveNodes(TreeNode * destination, TreeNode * source, size_t moveSize) {
  assert(moveSize % 4 == 0);
  assert((((uintptr_t)source) % 4) == 0);
  assert((((uintptr_t)destination) % 4) == 0);

  uint32_t * src = reinterpret_cast<uint32_t *>(source);
  uint32_t * dst = reinterpret_cast<uint32_t *>(destination);
  size_t len = moveSize/4;

  if (Helpers::Rotate(dst, src, len)) {
    updateNodeForIdentifierFromNode(dst < src ? destination : source);
  }
}

#if POINCARE_TREE_LOG
void TreePool::flatLog(std::ostream & stream) {
  size_t size = static_cast<char *>(m_cursor) - static_cast<char *>(buffer());
  stream << "<TreePool format=\"flat\" size=\"" << size << "\">";
  for (TreeNode * node : allNodes()) {
    node->log(stream, false);
  }
  stream << "</TreePool>";
  stream << std::endl;
}

void TreePool::treeLog(std::ostream & stream) {
  stream << "<TreePool format=\"tree\" size=\"" << (int)(m_cursor-buffer()) << "\">";
  for (TreeNode * node : roots()) {
    node->log(stream, true);
  }
  stream << "</TreePool>";
  stream << std::endl;
}

#endif

int TreePool::numberOfNodes() const {
  int count = 0;
  TreeNode * firstNode = first();
  TreeNode * lastNode = last();
  while (firstNode != lastNode) {
    count++;
    firstNode = firstNode->next();
  }
  return count;
}

void * TreePool::alloc(size_t size) {
  size = Helpers::AlignedSize(size, ByteAlignment);
  if (m_cursor >= buffer() + BufferSize || m_cursor + size > buffer() + BufferSize) {
    ExceptionCheckpoint::Raise();
  }
  void * result = m_cursor;
  m_cursor += size;
  return result;
}

void TreePool::dealloc(TreeNode * node, size_t size) {
  size = Helpers::AlignedSize(size, ByteAlignment);
  char * ptr = reinterpret_cast<char *>(node);
  assert(ptr >= buffer() && ptr < m_cursor);

  // Step 1 - Compact the pool
  memmove(
    ptr,
    ptr + size,
    m_cursor - (ptr + size)
  );
  m_cursor -= size;

  // Step 2: Update m_nodeForIdentifier for all nodes downstream
  updateNodeForIdentifierFromNode(node);
}

void TreePool::addGhostChildrenAndRename(TreeNode * node) {
  /* Ensure the pool is syntaxically correct by creating ghost children for
   * nodes that have a fixed, non-zero number of children. */
  for (int i = 0; i < node->numberOfChildren(); i++) {
    TreeNode * ghost = createTreeNode<GhostNode>();
    ghost->retain();
    move(node->next(), ghost, 0);
  }
  node->rename(generateIdentifier(), false);
}

void TreePool::discardTreeNode(TreeNode * node) {
  int nodeIdentifier = node->identifier();
  size_t size = node->size();
  node->~TreeNode();
  dealloc(node, size);
  freeIdentifier(nodeIdentifier);
}

void TreePool::registerNode(TreeNode * node) {
  int nodeID = node->identifier();
  if (nodeID >= 0 && nodeID < MaxNumberOfNodes) {
    m_nodeForIdentifier[nodeID] = node;
  }
}

void TreePool::updateNodeForIdentifierFromNode(TreeNode * node) {
  for (TreeNode * n : Nodes(node)) {
    m_nodeForIdentifier[n->identifier()] = n;
  }
}

void TreePool::freePoolFromNode(TreeNode * firstNodeToDiscard) {
  assert(firstNodeToDiscard != nullptr);
  assert(firstNodeToDiscard >= first());
  assert(firstNodeToDiscard <= last());

  if (firstNodeToDiscard < last()) {
    // There should be no tree that continues into the pool zone to discard
    assert(firstNodeToDiscard->parent() == nullptr);
  }
  TreeNode * currentNode = firstNodeToDiscard;
  TreeNode * lastNode = last();
  while (currentNode < lastNode) {
    freeIdentifier(currentNode->identifier());
    currentNode = currentNode->next();
  }
  m_cursor = reinterpret_cast<char *>(firstNodeToDiscard);
}

template HorizontalLayoutNode * Poincare::TreePool::createTreeNode<HorizontalLayoutNode>(size_t size);
template EmptyLayoutNode * Poincare::TreePool::createTreeNode<EmptyLayoutNode>(size_t size);
template CharLayoutNode * Poincare::TreePool::createTreeNode<CharLayoutNode>(size_t size);
template VerticalOffsetLayoutNode * Poincare::TreePool::createTreeNode<VerticalOffsetLayoutNode>(size_t size);
template NthRootLayoutNode * Poincare::TreePool::createTreeNode<NthRootLayoutNode>(size_t size);
template FractionLayoutNode * Poincare::TreePool::createTreeNode<FractionLayoutNode>(size_t size);
template LeftParenthesisLayoutNode * Poincare::TreePool::createTreeNode<LeftParenthesisLayoutNode>(size_t size);
template RightParenthesisLayoutNode * Poincare::TreePool::createTreeNode<RightParenthesisLayoutNode>(size_t size);
template AbsoluteValueNode * Poincare::TreePool::createTreeNode<AbsoluteValueNode>(size_t size);
template AdditionNode * Poincare::TreePool::createTreeNode<AdditionNode>(size_t size);
template ArcCosineNode * Poincare::TreePool::createTreeNode<ArcCosineNode>(size_t size);
template ArcSineNode * Poincare::TreePool::createTreeNode<ArcSineNode>(size_t size);
template ArcTangentNode * Poincare::TreePool::createTreeNode<ArcTangentNode>(size_t size);
template UndefinedNode * Poincare::TreePool::createTreeNode<UndefinedNode>(size_t size);
template BinomialCoefficientNode * Poincare::TreePool::createTreeNode<BinomialCoefficientNode>(size_t size);
template CeilingNode * Poincare::TreePool::createTreeNode<CeilingNode>(size_t size);
template OppositeNode * Poincare::TreePool::createTreeNode<OppositeNode>(size_t size);
template PowerNode * Poincare::TreePool::createTreeNode<PowerNode>(size_t size);
template ComplexArgumentNode * Poincare::TreePool::createTreeNode<ComplexArgumentNode>(size_t size);
template ConfidenceIntervalNode * Poincare::TreePool::createTreeNode<ConfidenceIntervalNode>(size_t size);
template ConjugateNode * Poincare::TreePool::createTreeNode<ConjugateNode>(size_t size);
template ConstantNode * Poincare::TreePool::createTreeNode<ConstantNode>(size_t size);
template CosineNode * Poincare::TreePool::createTreeNode<CosineNode>(size_t size);
template DecimalNode * Poincare::TreePool::createTreeNode<DecimalNode>(size_t size);
template DerivativeNode * Poincare::TreePool::createTreeNode<DerivativeNode>(size_t size);
template DeterminantNode * Poincare::TreePool::createTreeNode<DeterminantNode>(size_t size);
template DivisionNode * Poincare::TreePool::createTreeNode<DivisionNode>(size_t size);
template DivisionQuotientNode * Poincare::TreePool::createTreeNode<DivisionQuotientNode>(size_t size);
template DivisionRemainderNode * Poincare::TreePool::createTreeNode<DivisionRemainderNode>(size_t size);
template EmptyExpressionNode * Poincare::TreePool::createTreeNode<EmptyExpressionNode>(size_t size);
template FunctionNode * Poincare::TreePool::createTreeNode<FunctionNode>(size_t size);
template HyperbolicArcCosineNode * Poincare::TreePool::createTreeNode<HyperbolicArcCosineNode>(size_t size);
template HyperbolicArcSineNode * Poincare::TreePool::createTreeNode<HyperbolicArcSineNode>(size_t size);
template HyperbolicArcTangentNode * Poincare::TreePool::createTreeNode<HyperbolicArcTangentNode>(size_t size);
template HyperbolicCosineNode * Poincare::TreePool::createTreeNode<HyperbolicCosineNode>(size_t size);
template SimplePredictionIntervalNode * Poincare::TreePool::createTreeNode<SimplePredictionIntervalNode>(size_t size);
template HyperbolicSineNode * Poincare::TreePool::createTreeNode<HyperbolicSineNode>(size_t size);
template HyperbolicTangentNode * Poincare::TreePool::createTreeNode<HyperbolicTangentNode>(size_t size);
template LogarithmNode<2> * Poincare::TreePool::createTreeNode<LogarithmNode<2> >(size_t size);
template SymbolNode * Poincare::TreePool::createTreeNode<SymbolNode>(size_t size);
template LogarithmNode<1> * Poincare::TreePool::createTreeNode<LogarithmNode<1> >(size_t size);
template ParenthesisNode * Poincare::TreePool::createTreeNode<ParenthesisNode>(size_t size);
template StoreNode * Poincare::TreePool::createTreeNode<StoreNode>(size_t size);
template EqualNode * Poincare::TreePool::createTreeNode<EqualNode>(size_t size);
template FactorNode * Poincare::TreePool::createTreeNode<FactorNode>(size_t size);
template FactorialNode * Poincare::TreePool::createTreeNode<FactorialNode>(size_t size);
template FloorNode * Poincare::TreePool::createTreeNode<FloorNode>(size_t size);
template FracPartNode * Poincare::TreePool::createTreeNode<FracPartNode>(size_t size);
template MatrixNode * Poincare::TreePool::createTreeNode<MatrixNode>(size_t size);
template FloatNode<double> * Poincare::TreePool::createTreeNode<FloatNode<double> >(size_t size);
template GreatCommonDivisorNode * Poincare::TreePool::createTreeNode<GreatCommonDivisorNode>(size_t size);
template ImaginaryPartNode * Poincare::TreePool::createTreeNode<ImaginaryPartNode>(size_t size);
template IntegralNode * Poincare::TreePool::createTreeNode<IntegralNode>(size_t size);
template LeastCommonMultipleNode * Poincare::TreePool::createTreeNode<LeastCommonMultipleNode>(size_t size);
template MatrixDimensionNode * Poincare::TreePool::createTreeNode<MatrixDimensionNode>(size_t size);
template MatrixInverseNode * Poincare::TreePool::createTreeNode<MatrixInverseNode>(size_t size);
template MatrixTraceNode * Poincare::TreePool::createTreeNode<MatrixTraceNode>(size_t size);
template MatrixTransposeNode * Poincare::TreePool::createTreeNode<MatrixTransposeNode>(size_t size);
template MultiplicationNode * Poincare::TreePool::createTreeNode<MultiplicationNode>(size_t size);
template NaperianLogarithmNode * Poincare::TreePool::createTreeNode<NaperianLogarithmNode>(size_t size);
template NthRootNode * Poincare::TreePool::createTreeNode<NthRootNode>(size_t size);
template InfinityNode * Poincare::TreePool::createTreeNode<InfinityNode>(size_t size);
template PermuteCoefficientNode * Poincare::TreePool::createTreeNode<PermuteCoefficientNode>(size_t size);
template PredictionIntervalNode * Poincare::TreePool::createTreeNode<PredictionIntervalNode>(size_t size);
template ProductNode * Poincare::TreePool::createTreeNode<ProductNode>(size_t size);
template RandintNode * Poincare::TreePool::createTreeNode<RandintNode>(size_t size);
template RandomNode * Poincare::TreePool::createTreeNode<RandomNode>(size_t size);
template RationalNode * Poincare::TreePool::createTreeNode<RationalNode>(size_t size);
template RealPartNode * Poincare::TreePool::createTreeNode<RealPartNode>(size_t size);
template RoundNode * Poincare::TreePool::createTreeNode<RoundNode>(size_t size);
template SineNode * Poincare::TreePool::createTreeNode<SineNode>(size_t size);
template SquareRootNode * Poincare::TreePool::createTreeNode<SquareRootNode>(size_t size);
template SubtractionNode * Poincare::TreePool::createTreeNode<SubtractionNode>(size_t size);
template SumNode * Poincare::TreePool::createTreeNode<SumNode>(size_t size);
template TangentNode * Poincare::TreePool::createTreeNode<TangentNode>(size_t size);
template GhostNode * Poincare::TreePool::createTreeNode<GhostNode>(size_t size);

template MatrixLayoutNode* TreePool::createTreeNode<MatrixLayoutNode>(size_t size);
template AbsoluteValueLayoutNode* TreePool::createTreeNode<AbsoluteValueLayoutNode>(size_t size);
template ComplexNode<float>* TreePool::createTreeNode<ComplexNode<float> >(size_t size);
template ComplexNode<double>* TreePool::createTreeNode<ComplexNode<double> >(size_t size);
template MatrixComplexNode<float>* TreePool::createTreeNode<MatrixComplexNode<float> >(size_t size);
template MatrixComplexNode<double>* TreePool::createTreeNode<MatrixComplexNode<double> >(size_t size);
template BinomialCoefficientLayoutNode* TreePool::createTreeNode<BinomialCoefficientLayoutNode>(size_t size);
template CeilingLayoutNode* TreePool::createTreeNode<CeilingLayoutNode>(size_t size);
template CondensedSumLayoutNode* TreePool::createTreeNode<CondensedSumLayoutNode>(size_t size);
template ConjugateLayoutNode* TreePool::createTreeNode<ConjugateLayoutNode>(size_t size);
template FloorLayoutNode* TreePool::createTreeNode<FloorLayoutNode>(size_t size);
template IntegralLayoutNode* TreePool::createTreeNode<IntegralLayoutNode>(size_t size);
template ProductLayoutNode* TreePool::createTreeNode<ProductLayoutNode>(size_t size);
template SumLayoutNode* TreePool::createTreeNode<SumLayoutNode>(size_t size);
template FloatNode<float>* TreePool::createTreeNode<FloatNode<float> >(size_t size);

template LeftSquareBracketLayoutNode* TreePool::createTreeNode<LeftSquareBracketLayoutNode>(size_t size);
template RightSquareBracketLayoutNode* TreePool::createTreeNode<RightSquareBracketLayoutNode>(size_t size);

template BlobNode* TreePool::createTreeNode<BlobNode>(size_t size);
template PairNode* TreePool::createTreeNode<PairNode>(size_t size);

}
