#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/code_point_layout.h>
#include <poincare_junior/src/memory/cache_pool.h>
#include <poincare_junior/src/memory/placeholder.h>

#include "node_iterator.h"

namespace PoincareJ {

#if POINCARE_MEMORY_TREE_LOG

void Node::log(std::ostream& stream, bool recursive, bool verbose,
               int indentation, Node* comparison) const {
  Indent(stream, indentation);
  if (comparison && !isIdenticalTo(comparison)) {
    stream << "<<<<<<<";
    log(stream, recursive, verbose, indentation);
    Indent(stream, indentation);
    stream << "=======";
    comparison->log(stream, recursive, verbose, indentation);
    Indent(stream, indentation);
    stream << ">>>>>>>";
    return;
  }
  stream << "<";
  logName(stream);
  if (verbose) {
    stream << " size=\"" << nodeSize() << "\"";
    stream << " address=\"" << m_block << "\"";
  }
  logAttributes(stream);
  bool tagIsClosed = false;
  if (recursive) {
    for (const auto [child, index] :
         NodeIterator::Children<Forward, NoEditable>(this)) {
      if (!tagIsClosed) {
        stream << ">";
        tagIsClosed = true;
      }
      child->log(stream, recursive, verbose, indentation + 1,
                 comparison ? comparison->childAtIndex(index) : comparison);
    }
  }
  if (tagIsClosed) {
    Indent(stream, indentation);
    stream << "</";
    logName(stream);
    stream << ">";
  } else {
    stream << "/>";
  }
}

void Node::logName(std::ostream& stream) const {
  constexpr const char* names[] = {
      // Respect the order of BlockType
      "Zero",
      "One",
      "Two",
      "Half",
      "MinusOne",
      "IntegerShort",
      "IntegerPosBig",
      "IntegerNegBig",
      "RationalShort",
      "RationalPosBig",
      "RationalNegBig",
      "Float",
      "Multiplication",
      "Power",
      "Addition",
      "Factorial",
      "Division",
      "Constant",
      "UserSymbol",
      "Sine",
      "Cosine",
      "Tangent",
      "Abs",
      "ArcCosine",
      "ArcSine",
      "ArcTangent",
      "Derivative",
      "Exponential",
      "Ln",
      "Log",
      "Logarithm",
      "Polynomial",
      "Subtraction",
      "Trig",
      "TrigDiff",
      "UserFunction",
      "UserSequence",
      "List",
      "Set",
      "Undefined",
      "RackLayout",
      "FractionLayout",
      "ParenthesisLayout",
      "VerticalOffsetLayout",
      "CodePointLayout",
      "TreeBorder",
      "Placeholder",
      "SystemList",
  };
  static_assert(sizeof(names) / sizeof(const char*) ==
                static_cast<uint8_t>(BlockType::NumberOfTypes));
  stream << names[static_cast<uint8_t>(*m_block)];
}

void Node::logAttributes(std::ostream& stream) const {
  if (block()->isNAry()) {
    stream << " numberOfChildren=\"" << numberOfChildren() << "\"";
    if (type() == BlockType::Polynomial) {
      for (int i = 0; i < Polynomial::NumberOfTerms(this); i++) {
        stream << " exponent" << i << "=\""
               << static_cast<int>(
                      static_cast<uint8_t>(*(block()->nextNth(2 + i))))
               << "\"";
      }
    }
    return;
  }
  if (block()->isNumber() || type() == BlockType::Constant) {
    stream << " value=\"" << Approximation::To<float>(this) << "\"";
    return;
  }
  if (block()->isUserNamed() || type() == BlockType::CodePointLayout) {
    char buffer[64];
    (block()->isUserNamed() ? Symbol::GetName : CodePointLayout::GetName)(
        this, buffer, sizeof(buffer));
    stream << " value=\"" << buffer << "\"";
    return;
  }
  if (type() == BlockType::Placeholder) {
    stream << " tag=" << static_cast<int>(Placeholder::NodeToTag(this));
    stream << " filter=" << static_cast<int>(Placeholder::NodeToFilter(this));
    return;
  }
}

void Node::logBlocks(std::ostream& stream, bool recursive,
                     int indentation) const {
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
  stream << "[";
  logName(stream);
  stream << "]";
  int size = nodeSize();
  if (size > 1) {
    for (int i = 1; i < size - 1; i++) {
      stream << "[" << static_cast<int>(static_cast<uint8_t>(m_block[i]))
             << "]";
    }
    stream << "[";
    logName(stream);
    stream << "]";
  }
  stream << "\n";
  if (recursive) {
    indentation += 1;
    for (const auto [child, index] :
         NodeIterator::Children<Forward, NoEditable>(this)) {
      child->logBlocks(stream, recursive, indentation);
    }
  }
}

#endif

void Node::copyTreeTo(void* address) const {
  memcpy(address, m_block, treeSize());
}

const Node* Node::nextNode() const {
  assert(canNavigateNext());
  return Node::FromBlocks(m_block + nodeSize());
}

const Node* Node::previousNode() const {
  if (!canNavigatePrevious()) {
    return nullptr;
  }
  int previousSize =
      static_cast<const TypeBlock*>(m_block->previous())->nodeSize(false);
  return Node::FromBlocks(m_block - previousSize);
}

const Node* Node::previousTree() const { return previousRelative(false); }

const Node* Node::parent() const { return previousRelative(true); }

const Node* Node::root() const {
  const Node* ancestor = this;
  while (ancestor->parent() != nullptr) {
    ancestor = ancestor->parent();
  }
  return ancestor;
}

const Node* Node::commonAncestor(const Node* child1, const Node* child2) const {
  /* This method find the common ancestor of child1 and child2 within this tree
   * it does without going backward at any point. This tree is parsed until the
   * last node owning both childs is found. */
  const TypeBlock* block1 = child1->block();
  const TypeBlock* block2 = child2->block();
  if (block1 > block2) {
    return commonAncestor(child2, child1);
  }
  assert(block1 <= block2);
  if (block1 < block()) {
    return nullptr;
  }
  const Node* parent = nullptr;
  const Node* node = this;
  while (true) {
    assert(block1 >= node->block());
    const Node* nodeNextTree = node->nextTree();
    const bool descendant1 = block1 < nodeNextTree->block();
    const bool descendant2 = block2 < nodeNextTree->block();
    if (!descendant1) {
      // Neither children are descendants
      if (!parent) {
        // node is the root, no ancestors can be found
        return nullptr;
      }
      // Try node's next sibling
      node = nodeNextTree;
      continue;
    }
    if (!descendant2) {
      // Only child1 is descendant, parent is the common ancestor
      return parent;
    }
    if (block1 == node->block()) {
      // Either node or parent is the ancestor
      return node;
    }
    // Both children are in this tree, try node's first child
    parent = node;
    node = node->nextNode();
  }
  assert(false);
  return nullptr;
}

const Node* Node::parentOfDescendant(const Node* descendant,
                                     int* position) const {
  /* This method find the parent of child within this tree without going
   * backward at any point. This tree is parsed until the last node owning the
   * child is found. This also find position in the parent. */
  *position = 0;
  const TypeBlock* descendantBlock = descendant->block();
  if (descendantBlock < block()) {
    return nullptr;
  }
  const Node* parent = nullptr;
  const Node* node = this;
  while (true) {
    assert(descendantBlock >= node->block());
    const Node* nodeNextTree = node->nextTree();
    if (descendantBlock >= nodeNextTree->block()) {
      // node is not descendant's ancestor
      if (!parent) {
        // node is the root, no parent can be found
        return nullptr;
      }
      // Try node's next sibling
      *position = *position + 1;
      node = nodeNextTree;
      continue;
    }
    if (descendantBlock == node->block()) {
      return parent;
    }
    // descendant is in this tree, try node's first child
    parent = node;
    *position = 0;
    node = node->nextNode();
  }
  assert(false);
  return nullptr;
}

int Node::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  const Node* nextTreeNode = nextTree();
  const Node* currentNode = nextNode();
  while (currentNode != nextTreeNode) {
    result++;
    currentNode = currentNode->nextNode();
  }
  return result;
}

const Node* Node::childAtIndex(int i) const {
  for (const auto [child, index] :
       NodeIterator::Children<Forward, NoEditable>(this)) {
    if (index == i) {
      return child;
    }
  }
  return nullptr;
}

int Node::indexOfChild(const Node* child) const {
  for (const auto [c, index] :
       NodeIterator::Children<Forward, NoEditable>(this)) {
    if (child == c) {
      return index;
    }
  }
  return -1;
}

int Node::indexInParent() const {
  const Node* p = parent();
  if (!p) {
    return -1;
  }
  return p->indexOfChild(this);
}

bool Node::hasChild(const Node* child) const {
  return indexOfChild(child) >= 0;
}

bool Node::hasAncestor(const Node* node, bool includeSelf) const {
  const Node* ancestor = this;
  do {
    if (ancestor == node) {
      return includeSelf || (ancestor != this);
    }
    ancestor = ancestor->parent();
  } while (ancestor);
  return false;
}

bool Node::hasSibling(const Node* sibling) const {
  const Node* p = parent();
  if (!p) {
    return false;
  }
  for (const auto& [child, index] :
       NodeIterator::Children<Forward, NoEditable>(p)) {
    if (child == sibling) {
      return true;
    }
  }
  return false;
}

void Node::recursivelyGet(InPlaceConstTreeFunction treeFunction) const {
  for (const auto& [child, index] :
       NodeIterator::Children<Forward, NoEditable>(this)) {
    child->recursivelyGet(treeFunction);
  }
  (*treeFunction)(this);
}

EditionReference Node::clone() const {
  return EditionPool::sharedEditionPool()->clone(this);
}

/* When navigating between nodes, ensure that no undefined node is reached.
 * Also ensure that there is no navigation:
 * - crossing the borders of the CachePool
 * - going across a TreeBorder
 * Node*::nextNode asserts that such events don't occur whereas
 * Node*::previousNode tolerates (and handles) them to identify root nodes.
 * Here are the situations that indicate navigation must stop:
 * nextNode:
 * (1) From a TreeBorder
 * (2) To the Cache first block
 * (3) From the Edition pool last block
 * // (4) To the Cache last block / Edition pool first block
 *
 * previousNode:
 * (5) To a TreeBorder
 * (6) From the Cache first block
 * // (7) From the Edition pool last block
 * // (8) From the Cache last block / Edition pool first block
 *
 * Some notes :
 * - It is expected in (2) and (3) that any tree out of the pool is wrapped in
 *   TreeBorders.
 * - For both pools, last block represent the very first out of limit block.
 *   We need to call previousNode on them to access before last node so (7) and
 *   (8) are not checked.
 * - Cache last block is also Edition first block, and we need to call nextNode
 *   on before last node, so (4) is not checked.
 * - Source node is always expected to be defined. Allowing checks on
 *   nextNode's destination, but not previousNode's. */

bool Node::canNavigateNext() const {
  CachePool* cache(CachePool::sharedCachePool());
  return m_block->type() != BlockType::TreeBorder &&
         m_block + nodeSize() != cache->firstBlock() &&
         m_block != cache->editionPool()->lastBlock();
}

bool Node::canNavigatePrevious() const {
  CachePool* cache(CachePool::sharedCachePool());
  BlockType destinationType =
      static_cast<const TypeBlock*>(m_block->previous())->type();
  return destinationType != BlockType::TreeBorder &&
         m_block != cache->firstBlock();
}

const Node* Node::previousRelative(bool parent) const {
  const Node* currentNode = this;
  const Node* closestSibling = nullptr;
  int nbOfChildrenToScan = 0;
  do {
    currentNode = currentNode->previousNode();
    if (!currentNode) {
      break;
    }
    nbOfChildrenToScan += currentNode->numberOfChildren() - 1;
    if (nbOfChildrenToScan == -1) {
      closestSibling = currentNode;
    }
  } while (nbOfChildrenToScan < 0);
  return parent ? currentNode : closestSibling;
}

// Node edition

void Node::cloneAt(const Node* nodeToClone, bool before, bool newIsTree) {
  Node* destination = before ? this : nextNode();
  size_t size = newIsTree ? nodeToClone->treeSize() : nodeToClone->nodeSize();
  EditionPool::sharedEditionPool()->insertBlocks(destination->block(),
                                                 nodeToClone->block(), size);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Insert", destination->block(), size);
#endif
}

void Node::moveAt(Node* nodeToMove, bool before, bool newIsTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Node* destination = before ? this : nextNode();
  size_t size = newIsTree ? nodeToMove->treeSize() : nodeToMove->nodeSize();
  assert(pool->contains(nodeToMove->block()));
  pool->moveBlocks(destination->block(), nodeToMove->block(), size);
#if POINCARE_POOL_VISUALIZATION
  Block* dst = destination->block();
  Block* addedBlock = dst >= nodeToMove->block() ? dst - size : dst;
  Log(LoggerType::Edition, "Insert", addedBlock, size, nodeToMove->block());
#endif
}

Node* Node::cloneOver(const Node* newNode, bool oldIsTree, bool newIsTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Node* oldNode = this;
  int oldSize = oldIsTree ? oldNode->treeSize() : oldNode->nodeSize();
  int newSize = newIsTree ? newNode->treeSize() : newNode->nodeSize();
  Block* oldBlock = oldNode->block();
  const Block* newBlock = newNode->block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return Node::FromBlocks(oldBlock);
  }
  size_t minSize = std::min(oldSize, newSize);
  pool->replaceBlocks(oldBlock, newBlock, minSize);
  if (oldSize > newSize) {
    pool->removeBlocks(oldBlock + minSize, oldSize - newSize);
  } else {
    pool->insertBlocks(oldBlock + minSize, newBlock + minSize,
                       newSize - oldSize);
  }
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Replace", oldBlock, newSize);
#endif
  return Node::FromBlocks(oldBlock);
}

Node* Node::moveOver(Node* newNode, bool oldIsTree, bool newIsTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Node* oldNode = this;
  int oldSize = oldIsTree ? oldNode->treeSize() : oldNode->nodeSize();
  int newSize = newIsTree ? newNode->treeSize() : newNode->nodeSize();
  Block* oldBlock = oldNode->block();
  Block* newBlock = newNode->block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return Node::FromBlocks(oldBlock);
  }
  Block* finalBlock = oldBlock;
  assert(pool->contains(newNode->block()));
  // Fractal scheme
  assert(!(newIsTree && oldNode->hasAncestor(newNode, true)));
  if (oldIsTree && newNode->hasAncestor(oldNode, true)) {
    oldSize -= newSize;
  }
  pool->moveBlocks(oldBlock, newBlock, newSize);
  if (oldBlock > newBlock) {
    finalBlock -= newSize;
  }
  pool->removeBlocks(finalBlock + newSize, oldSize);
#if POINCARE_POOL_VISUALIZATION
  if (oldBlock < newBlock) {
    newBlock -= oldSize;
  }
  Log(LoggerType::Edition, "Replace", finalBlock, newSize, newBlock);
#endif
  return Node::FromBlocks(finalBlock);
}

void Node::remove(bool isTree) {
  Block* b = block();
  size_t size = isTree ? treeSize() : nodeSize();
  EditionPool::sharedEditionPool()->removeBlocks(b, size);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Remove", nullptr, INT_MAX, b);
#endif
}

Node* Node::detach(bool isTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Block* destination = pool->lastBlock();
  size_t sizeToMove = isTree ? treeSize() : nodeSize();
  Block* source = block();
  pool->moveBlocks(destination, source, sizeToMove);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Detach", destination - sizeToMove, sizeToMove,
      source);
#endif
  return Node::FromBlocks(destination);
}

}  // namespace PoincareJ
