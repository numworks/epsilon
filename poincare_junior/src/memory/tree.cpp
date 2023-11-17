#include <poincare_junior/src/memory/cache_pool.h>

#include "node_iterator.h"

#if POINCARE_POOL_VISUALIZATION
#include <poincare_junior/include/poincare.h>
#endif

#if POINCARE_MEMORY_TREE_LOG
#include <ion/unicode/utf8_decoder.h>
#include <poincare_junior/include/layout.h>
#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/constant.h>
#include <poincare_junior/src/expression/matrix.h>
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/random.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/expression/variables.h>
#include <poincare_junior/src/layout/autocompleted_pair.h>
#include <poincare_junior/src/layout/code_point_layout.h>
#include <poincare_junior/src/layout/layoutter.h>
#include <poincare_junior/src/memory/placeholder.h>
#endif

namespace PoincareJ {

#if POINCARE_MEMORY_TREE_LOG

void Tree::log(std::ostream& stream, bool recursive, bool verbose,
               int indentation, const Tree* comparison) const {
  Indent(stream, indentation);
  if (comparison && !nodeIsIdenticalTo(comparison)) {
    stream << "<<<<<<<\n";
    log(stream, recursive, verbose, indentation);
    Indent(stream, indentation);
    stream << "=======\n";
    comparison->log(stream, recursive, verbose, indentation);
    Indent(stream, indentation);
    stream << ">>>>>>>\n";
    return;
  }
  stream << "<";
  logName(stream);
  if (verbose) {
    stream << " size=\"" << nodeSize() << "\"";
    stream << " address=\"" << this << "\"";
  }
  logAttributes(stream);
  bool tagIsClosed = false;
  if (recursive) {
    for (auto [child, index] : NodeIterator::Children<NoEditable>(this)) {
      if (!tagIsClosed) {
        stream << ">\n";
        tagIsClosed = true;
      }
      child->log(stream, recursive, verbose, indentation + 1,
                 comparison ? comparison->child(index) : comparison);
    }
  }
  if (tagIsClosed) {
    Indent(stream, indentation);
    stream << "</";
    logName(stream);
    stream << ">\n";
  } else {
    stream << "/>\n";
  }
}

void Tree::logName(std::ostream& stream) const {
  stream << TypeBlock::names[m_content];
}

void Tree::logAttributes(std::ostream& stream) const {
  if (type().isNAry()) {
    stream << " numberOfChildren=\"" << numberOfChildren() << "\"";
    if (isPolynomial()) {
      for (int i = 0; i < Polynomial::NumberOfTerms(this); i++) {
        stream << " exponent" << i << "=\""
               << static_cast<int>(nodeValue(1 + i)) << "\"";
      }
    }
    return;
  }
  if (isMatrix()) {
    stream << " numberOfRows=\"" << static_cast<int>(Matrix::NumberOfRows(this))
           << "\"";
    stream << " numberOfColumns=\""
           << static_cast<int>(Matrix::NumberOfColumns(this)) << "\"";
  }
  if (isConstant()) {
    char buffer[4];
    size_t size = UTF8Decoder::CodePointToChars(
        Constant::ToCodePoint(Constant::Type(this)), buffer, 4);
    buffer[size] = 0;
    stream << " type=\"" << buffer << "\"";
  }
  if (isNumber()) {
    stream << " value=\"" << Approximation::To<float>(this, nullptr) << "\"";
    return;
  }
  if (isUserNamed() || isCodePointLayout()) {
    char buffer[64];
    (isUserNamed() ? Symbol::GetName : CodePointLayout::GetName)(
        this, buffer, sizeof(buffer));
    stream << " value=\"" << buffer << "\"";
    return;
  }
  if (isVariable()) {
    stream << " id=" << static_cast<int>(Variables::Id(this));
    return;
  }
  if (isPlaceholder()) {
    stream << " tag=" << static_cast<int>(Placeholder::NodeToTag(this));
    stream << " filter=" << static_cast<int>(Placeholder::NodeToFilter(this));
    return;
  }
  if (isRandomNode()) {
    stream << " seed=" << static_cast<int>(Random::GetSeed(this));
    return;
  }
  if (isUnit()) {
    stream << " representativeId=" << static_cast<int>(nodeValue(0));
    stream << " prefixId=" << static_cast<int>(nodeValue(1));
    return;
  }
  if (isAutocompletedPair()) {
    stream << " leftIsTemporary="
           << AutocompletedPair::IsTemporary(this, Side::Left);
    stream << " rightIsTemporary="
           << AutocompletedPair::IsTemporary(this, Side::Right);
    return;
  }
}

void Tree::logSerialize(std::ostream& stream) const {
  EditionReference outputLayout = Layoutter::LayoutExpression(clone());
  assert(!outputLayout.isUninitialized());
  constexpr size_t bufferSize = 256;
  char buffer[bufferSize];
  *Layout::Serialize(outputLayout, buffer, buffer + bufferSize) = 0;
  outputLayout->removeTree();
  stream << buffer << "\n";
}

void Tree::logBlocks(std::ostream& stream, bool recursive,
                     int indentation) const {
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
  stream << "[";
  logName(stream);
  stream << "]";
  int size = nodeSize();
  for (int i = 0; i < size - 1; i++) {
    stream << "[" << static_cast<int>(static_cast<uint8_t>(m_valueBlocks[i]))
           << "]";
  }
  stream << "\n";
  if (recursive) {
    indentation += 1;
    for (const Tree* child : children()) {
      child->logBlocks(stream, recursive, indentation);
    }
  }
}

#endif

void Tree::copyTreeTo(void* address) const {
  memcpy(address, this, treeSize());
}

/* When navigating between nodes, assert that no undefined node is reached.
 * Also ensure that there is no navigation:
 * - crossing the borders of the CachePool
 * - going across a TreeBorder
 * Here are the situations indicating nextNode navigation must stop:
 * (1) From a TreeBorder
 * (2) To the Cache first block
 * (3) From the Edition pool last block
 * // (4) To the Cache last block / Edition pool first block
 *
 * Some notes :
 * - It is expected in (2) and (3) that any tree out of the pool ends with a
 *   TreeBorder block.
 * - For both pools, last block represent the very first out of limit block.
 * - Cache last block is also Edition first block, and we need to call nextNode
 *   on before last node, so (4) is not checked.
 * - Source node is always expected to be defined. Allowing checks on
 *   nextNode's destination. */

#if !PLATFORM_DEVICE
uint32_t Tree::nextNodeCount = 0;
uint32_t Tree::nextNodeInPoolCount = 0;
#endif

const Tree* Tree::nextNode() const {
#if ASSERTIONS
  assert(!isTreeBorder());
#endif
  assert(this + nodeSize() != CachePool::SharedCachePool->firstBlock());
  assert(this != SharedEditionPool->lastBlock());
#if !PLATFORM_DEVICE
  if (SharedEditionPool->firstBlock() <= this &&
      this <= SharedEditionPool->lastBlock()) {
    nextNodeInPoolCount++;
  }
  nextNodeCount++;
#endif
  return Tree::FromBlocks(this + nodeSize());
}

const Tree* Tree::commonAncestor(const Tree* child1, const Tree* child2) const {
  /* This method find the common ancestor of child1 and child2 within this
   * tree it does without going backward at any point. This tree is parsed
   * until the last node owning both childs is found. */
  const Block* block1 = child1->block();
  const Block* block2 = child2->block();
  if (block1 > block2) {
    return commonAncestor(child2, child1);
  }
  assert(block1 <= block2);
  if (block1 < block()) {
    return nullptr;
  }
  const Tree* parent = nullptr;
  const Tree* node = this;
  while (true) {
    assert(block1 >= node->block());
    const Tree* nodeNextTree = node->nextTree();
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

const Tree* Tree::parentOfDescendant(const Tree* descendant,
                                     int* position) const {
  /* This method find the parent of child within this tree without going
   * backward at any point. This tree is parsed until the last node owning the
   * child is found. This also find position in the parent. */
  *position = 0;
  const Block* descendantBlock = descendant->block();
  if (descendantBlock < block()) {
    return nullptr;
  }
  const Tree* parent = nullptr;
  const Tree* node = this;
  while (true) {
    assert(descendantBlock >= node->block());
    const Tree* nodeNextTree = node->nextTree();
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

int Tree::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  for (const Tree* currentNode : descendants()) {
    (void)currentNode;
    result++;
  }
  return result;
}

const Tree* Tree::child(int i) const {
  assert(i < numberOfChildren());
  const Tree* child = nextNode();
  for (; i > 0; i--) {
    child = child->nextTree();
  }
  return child;
}

int Tree::indexOfChild(const Tree* child) const {
  for (const auto [c, index] : NodeIterator::Children<NoEditable>(this)) {
    if (child == c) {
      return index;
    }
  }
  return -1;
}

bool Tree::hasChild(const Tree* child) const {
  return indexOfChild(child) >= 0;
}

bool Tree::hasAncestor(const Tree* node, bool includeSelf) const {
  if (this < node) {
    return false;
  }
  if (this == node) {
    return includeSelf;
  }
  return block() < node->block() + node->treeSize();
}

bool Tree::ApplyShallowInDepth(Tree* ref, ShallowOperation shallowOperation,
                               void* context, bool check) {
  bool changed = false;
  for (Tree* node : ref->selfAndDescendants()) {
    changed = shallowOperation(node, context) || changed;
    assert(!check || !shallowOperation(node, context));
  }
  return changed;
}

Tree* Tree::clone() const { return SharedEditionPool->clone(this); }
Tree* Tree::cloneNode() const { return SharedEditionPool->clone(this, false); }

// Tree edition

Tree* Tree::cloneAt(const Tree* nodeToClone, bool before, bool newIsTree,
                    bool at) {
  Tree* destination = before ? this : nextNode();
  size_t size = newIsTree ? nodeToClone->treeSize() : nodeToClone->nodeSize();
  SharedEditionPool->insertBlocks(destination->block(), nodeToClone->block(),
                                  size, at);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Insert", destination->block(), size);
#endif
  return destination;
}

Tree* Tree::moveAt(Tree* nodeToMove, bool before, bool newIsTree, bool at) {
  Tree* destination = before ? this : nextNode();
  size_t size = newIsTree ? nodeToMove->treeSize() : nodeToMove->nodeSize();
  assert(SharedEditionPool->contains(nodeToMove->block()));
  SharedEditionPool->moveBlocks(destination->block(), nodeToMove->block(), size,
                                at);
  Block* dst = destination->block();
  Block* addedBlock = dst > nodeToMove->block() ? dst - size : dst;
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Insert", addedBlock, size, nodeToMove->block());
#endif
  return Tree::FromBlocks(addedBlock);
}

Tree* Tree::cloneOver(const Tree* newNode, bool oldIsTree, bool newIsTree) {
  Tree* oldNode = this;
  int oldSize = oldIsTree ? oldNode->treeSize() : oldNode->nodeSize();
  int newSize = newIsTree ? newNode->treeSize() : newNode->nodeSize();
  Block* oldBlock = oldNode->block();
  const Block* newBlock = newNode->block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return Tree::FromBlocks(oldBlock);
  }
  size_t minSize = std::min(oldSize, newSize);
  SharedEditionPool->replaceBlocks(oldBlock, newBlock, minSize);
  if (oldSize > newSize) {
    SharedEditionPool->removeBlocks(oldBlock + minSize, oldSize - newSize);
  } else {
    SharedEditionPool->insertBlocks(oldBlock + minSize, newBlock + minSize,
                                    newSize - oldSize);
  }
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Replace", oldBlock, newSize);
#endif
  return Tree::FromBlocks(oldBlock);
}

Tree* Tree::moveOver(Tree* newNode, bool oldIsTree, bool newIsTree) {
  Tree* oldNode = this;
  int oldSize = oldIsTree ? oldNode->treeSize() : oldNode->nodeSize();
  int newSize = newIsTree ? newNode->treeSize() : newNode->nodeSize();
  Block* oldBlock = oldNode->block();
  Block* newBlock = newNode->block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return Tree::FromBlocks(oldBlock);
  }
  Block* finalBlock = oldBlock;
  assert(SharedEditionPool->contains(newNode->block()));
  // Fractal scheme
  assert(!(newIsTree && oldNode->hasAncestor(newNode, true)));
  if (oldIsTree && newNode->hasAncestor(oldNode, true)) {
    oldSize -= newSize;
  }
  SharedEditionPool->moveBlocks(oldBlock, newBlock, newSize);
  if (oldBlock > newBlock) {
    finalBlock -= newSize;
  }
  SharedEditionPool->removeBlocks(finalBlock + newSize, oldSize);
#if POINCARE_POOL_VISUALIZATION
  if (oldBlock < newBlock) {
    newBlock -= oldSize;
  }
  Log(LoggerType::Edition, "Replace", finalBlock, newSize, newBlock);
#endif
  return Tree::FromBlocks(finalBlock);
}

void Tree::remove(bool isTree) {
  Block* b = block();
  size_t size = isTree ? treeSize() : nodeSize();
  SharedEditionPool->removeBlocks(b, size);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Remove", nullptr, INT_MAX, b);
#endif
}

Tree* Tree::detach(bool isTree) {
  Block* destination = SharedEditionPool->lastBlock();
  size_t sizeToMove = isTree ? treeSize() : nodeSize();
  Block* source = block();
  SharedEditionPool->moveBlocks(destination, source, sizeToMove, true);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Detach", destination - sizeToMove, sizeToMove,
      source);
#endif
  return Tree::FromBlocks(destination - sizeToMove);
}

void SwapTrees(Tree* u, Tree* v) {
  if (u->block() > v->block()) {
    return SwapTrees(v, u);
  }
  v->moveTreeBeforeNode(u);
  u->moveTreeBeforeNode(v);
}

void SwapTrees(Tree** u, Tree** v) {
  if (*u > *v) {
    SwapTrees(v, u);
  }
  Block* newV = (*v)->block() - ((*u)->treeSize() - (*v)->treeSize());
  SwapTrees(*u, *v);
  *v = *u;
  *u = Tree::FromBlocks(newV);
}

}  // namespace PoincareJ
