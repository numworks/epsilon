#include "tree.h"

#include <ion.h>
#include <omg/global_box.h>

#include "tree_ref.h"

#if POINCARE_TREE_STACK_VISUALIZATION
#include <poincare/src/memory/visualization.h>
#endif

#if POINCARE_TREE_LOG
#include <omg/utf8_decoder.h>
#include <poincare/src/expression/approximation.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/polynomial.h>
#include <poincare/src/expression/random.h>
#include <poincare/src/expression/symbol.h>
#include <poincare/src/expression/variables.h>
#include <poincare/src/layout/autocompleted_pair.h>
#include <poincare/src/layout/code_point_layout.h>
#include <poincare/src/layout/layout_serializer.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/layout/sequence.h>
#include <poincare/src/layout/vertical_offset.h>

#include "placeholder.h"
#endif

#if ASSERTIONS
#include <poincare/src/expression/parametric.h>
#endif

namespace Poincare::Internal {

#if POINCARE_TREE_LOG

void Indent(std::ostream& stream, int indentation) {
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
}

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
    for (IndexedChild<const Tree*> child : indexedChildren()) {
      if (!tagIsClosed) {
        stream << ">\n";
        tagIsClosed = true;
      }
      child->log(stream, recursive, verbose, indentation + 1,
                 comparison ? comparison->child(child.index) : comparison);
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
  if (isNumber()) {
    stream << " value=\""
           << Approximation::To<float>(this, Approximation::Parameters{})
           << "\"";
  }
  if (isVar()) {
    stream << " id=\"" << static_cast<int>(Variables::Id(this)) << "\"";
    stream << " sign=\"";
    Variables::GetComplexSign(this).log(stream, false);
    stream << "\"";
  }
  if (isUserNamed()) {
    stream << " value=\"" << Symbol::GetName(this) << "\"";
  }
  if (isCodePointLayout()) {
    char buffer[64];
    CodePointLayout::CopyName(this, buffer, sizeof(buffer));
    stream << " value=\"" << buffer << "\"";
  }
  if (isPlaceholder()) {
    stream << " tag=\"" << static_cast<int>(Placeholder::NodeToTag(this))
           << "\"";
    stream << " filter=\"" << static_cast<int>(Placeholder::NodeToFilter(this))
           << "\"";
  }
  if (isRandomized()) {
    stream << " seed=\"" << static_cast<int>(Random::GetSeed(this)) << "\"";
  }
  if (isUnit()) {
    stream << " representativeId=\"" << static_cast<int>(nodeValue(0)) << "\"";
    stream << " prefixId=\"" << static_cast<int>(nodeValue(1)) << "\"";
  }
  if (isAutocompletedPair()) {
    stream << " leftIsTemporary=\""
           << AutocompletedPair::IsTemporary(this, Side::Left) << "\"";
    stream << " rightIsTemporary=\""
           << AutocompletedPair::IsTemporary(this, Side::Right) << "\"";
  }
  if (isDiffLayout()) {
    stream << " isNthDerivative=\"" << toDiffLayoutNode()->isNthDerivative
           << "\"";
  }
  if (isVerticalOffsetLayout()) {
    stream << " isSubscript=\"" << VerticalOffset::IsSubscript(this) << "\"";
    stream << " isPrefix=\"" << VerticalOffset::IsPrefix(this) << "\"";
  }
  if (isSequenceLayout()) {
    stream << " firstRank=\""
           << static_cast<int>(SequenceLayout::FirstRank(this)) << "\"";
  }
}

void Tree::logSerialize(std::ostream& stream) const {
  assert(isExpression() || isRackOrLayout());
  Tree* layout = isExpression()
                     ? Layouter::LayoutExpression(cloneTree(), true, false)
                     : cloneTree();
  assert(layout);
  constexpr size_t bufferSize = 1024;
  char buffer[bufferSize];
  LayoutSerializer::Serialize(layout, buffer);
  layout->removeTree();
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
 * - crossing the borders of the TreeStack
 * - going across a TreeBorder
 * Here are the situations indicating nextNode navigation must stop:
 * (1) From a TreeBorder
 * (2) To the TreeStack's first block
 * (3) From the TreeStack's last block
 *
 * Some notes :
 * - It is expected in (2) and (3) that any tree out of the stack ends with a
 *   TreeBorder block.
 * - In the TreeStack, the last block is the very first out of limit block.
 * - Source node is always expected to be defined. Allowing checks on
 *   nextNode's destination. */

#if POINCARE_METRICS
uint32_t Tree::nextNodeCount = 0;
uint32_t Tree::nextNodeInTreeStackCount = 0;
#endif

bool Tree::treeIsIdenticalTo(const Tree* other) const {
  if (this == other) {
    return true;
  }
  const Tree* self = this;
  // Overflow is valid here
  size_t offset = self - other;
  int nbOfNodeToScan = 1;
  while (nbOfNodeToScan > 0) {
    const Tree* selfNextNode = self->nextNode();
    const Tree* otherNextNode = other->nextNode();
    /* This next if is basically an inlined version of nodeIsIdenticalTo
     * selfNextNode - otherNextNode != offset is the faster equivalent of
     * other->nodeSize() != self->nodeSize() */
    if (static_cast<size_t>(selfNextNode - otherNextNode) != offset ||
        memcmp(self, other, selfNextNode - self) != 0) {
      return false;
    }
    nbOfNodeToScan += self->numberOfChildren() - 1;
    self = selfNextNode;
    other = otherNextNode;
  }
  return true;
}

bool Tree::nodeIsIdenticalTo(const Tree* other) const {
  size_t size = nodeSize();
  return size == other->nodeSize() && memcmp(this, other, size) == 0;
}

const Tree* NextTreeNoCache(const Tree* result) {
  assert(result != nullptr);
  int nbOfChildrenToScan = 1;
  while (nbOfChildrenToScan > 0) {
    /* This while loop is equivalent to the following code:
     *
     * nbOfChildrenToScan += result->numberOfChildren() - 1;
     * result = result->nextNode();
     *
     * To optimize the [numberOfChildren] and [nodeSize] data is gather in a
     * single call to [numberOfChildrenAndNodeSize] */

    Tree::NodeInfo cs = result->numberOfChildrenAndNodeSize();
    assert(cs.nodeSize == result->nodeSize());
    assert(cs.numberOfChildren == result->numberOfChildren());
    nbOfChildrenToScan += cs.numberOfChildren - 1;
    result = result->nextNode(cs.nodeSize);
  }
  return result;
}

template <size_t k_size>
class Cache {
 public:
  bool FindInCache(const Tree* key, const Tree** result) {
    for (size_t i = 0; i < m_cacheLength; ++i) {
      if (m_data[i].key == key) {
        assert(m_data[i].value == NextTreeNoCache(key));
        *result = m_data[i].value;
        return true;
      }
    }
    return false;
  }

  void UpdateCache(const Tree* key, const Tree* value) {
    m_data[m_cacheIndex].value = value;
    m_data[m_cacheIndex].key = key;
    ++m_cacheIndex %= k_size;
    if (m_cacheLength < k_size) {
      ++m_cacheLength;
    }
    assert(m_cacheIndex < k_size && m_cacheIndex <= m_cacheLength);
  }

  void ResetCache() {
    m_cacheLength = 0;
    m_cacheIndex = 0;
  }

  struct Entry {
    const Tree* key;
    const Tree* value;
  };

 private:
  Entry m_data[k_size];
  size_t m_cacheLength;
  size_t m_cacheIndex;
};

Cache<20> TreeStackCache;
Cache<5> ReadOnlyCache;

/* We tried using the [after] argument to only clear the cache located after
 * this address. Ensuring that cache information from trees that did not change
 * stayed on cache. But this lead to a drastic slow down on the cache reset. */
void Tree::ResetCache(const void* after) { TreeStackCache.ResetCache(); }

template <size_t s>
const Tree* NextTreeWithCache(const Tree* e, Cache<s>* cache) {
  const Tree* result;
  if (!cache->FindInCache(e, &result)) {
    result = NextTreeNoCache(e);
    cache->UpdateCache(e, result);
  }
  return result;
}

const Tree* Tree::nextTree() const {
  if (SharedTreeStack->contains(this)) {
    return NextTreeWithCache(this, &TreeStackCache);
  } else if (Ion::ReadOnlyMemory::IncludesAddress(this)) {
    return NextTreeWithCache(this, &ReadOnlyCache);
  }
  return NextTreeNoCache(this);
}

#if POINCARE_TREE_LOG
template <size_t t>
void LogC(Cache<t>* c) {
  std::cout << "Cache has " << c->m_cacheLength << " elements\n";
  for (int i = 0; i < c->m_cacheLength; ++i) {
    std::cout << c->m_data[i].key << " => " << c->m_data[i].value << std::endl;
  }
}
#endif

const Tree* Tree::nextNode(size_t nodeSize) const {
  assert(!isTreeBorder());
  assert(nodeSize == Tree::nodeSize());
  assert(this + nodeSize != SharedTreeStack->firstBlock());
  assert(this != SharedTreeStack->lastBlock());
#if POINCARE_METRICS
  if (SharedTreeStack->firstBlock() <= this &&
      this <= SharedTreeStack->lastBlock()) {
    nextNodeInTreeStackCount++;
  }
  nextNodeCount++;
#endif
  return Tree::FromBlocks(this + nodeSize);
}

uint32_t Tree::hash() const {
  return Ion::crc32Byte(reinterpret_cast<const uint8_t*>(this), treeSize());
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

struct ParentAndPosition {
  const Tree* parent;
  int position;
};

/* Return [{parent, position}] if [descendant] is in [root], else return
 * [{nullptr, 0}].
 * If [rootNextTree] is set, then it is [root->nextTree()]. */
static ParentAndPosition ParentOfDescendantAux(
    const Tree* root, const Tree* descendant,
    const Tree** rootNextTree = nullptr) {
  struct ParentInfo {
    const Tree* parent;
    int remainingChildren;
    int numberOfChildren;
  };
  // Arbitrary size, increase to make less recursive calls
  constexpr size_t k_size = 10;
  // Behaves like a LIFO pile
  ParentInfo pile[k_size];
  Tree::NodeInfo rootInfo = root->numberOfChildrenAndNodeSize();
  const Tree* node = root->nextNode(rootInfo.nodeSize);
  int i = 0;
  pile[0] = {root, rootInfo.numberOfChildren, rootInfo.numberOfChildren};
  while (true) {
    /* Remove parents with 0 [remainingChildren], indeed the [node]'s parent is
     * the last element of [pile] with [remainingChildren] > 0  */
    while (pile[i].remainingChildren == 0) {
      i--;
      if (i < 0) {
        // descendant not in root
        assert(node == root->nextTree());
        if (rootNextTree) {
          // Update [rootNextTree], this is an optimization
          *rootNextTree = node;
        }
        return {nullptr, 0};
      }
    }
    if (node == descendant) {
      return {pile[i].parent,
              pile[i].numberOfChildren - pile[i].remainingChildren};
    }
    pile[i].remainingChildren--;

    Tree::NodeInfo info = node->numberOfChildrenAndNodeSize();
    /* Avoid increasing [i] if [numberOfChildren] is 0 as it will be decreased
     * at the start of next loop */
    if (info.numberOfChildren > 0) {
      if (i >= static_cast<int>(k_size) - 1) {
        // No space left in local [pile], recursive call to continue
        ParentAndPosition pp = ParentOfDescendantAux(node, descendant, &node);
        if (pp.parent != nullptr) {
          // Successfully found parent in recursive call: return
          return pp;
        }
        /* [node] has been updated by the recursive call to be the next tree
         * to inspect */
        continue;
      }
      pile[++i] = {node, info.numberOfChildren, info.numberOfChildren};
    }
    node = node->nextNode(info.nodeSize);
  }
}

const Tree* Tree::parentOfDescendant(const Tree* descendant,
                                     int* position) const {
  /* This method finds the parent of child within this tree without going
   * backward at any point. This tree is parsed until the last node owning the
   * child is found. This also finds the position of the child in the parent. */
  if (descendant < this) {
    return nullptr;
  }
  ParentAndPosition pp = ParentOfDescendantAux(this, descendant);
  *position = pp.position;
  return pp.parent;  // May be nullptr
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
  assert(i >= 0 && i < numberOfChildren());
  if (i == 0) {
    /* For some reason (-Os ?) the compiler doesn't try to unroll the loop when
     * it sees a call to child(0). */
    return nextNode();
  }
  const Tree* child = nextNode();
  for (; i > 0; i--) {
    child = child->nextTree();
  }
  return child;
}

int Tree::indexOfChild(const Tree* child) const {
  for (IndexedChild<const Tree*> c : indexedChildren()) {
    if (child == static_cast<const Tree*>(c)) {
      return c.index;
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

bool Tree::ApplyShallowTopDown(Tree* t, ShallowOperation shallowOperation,
                               void* context, bool check) {
  bool changed = false;
  for (Tree* node : t->selfAndDescendants()) {
    changed = shallowOperation(node, context) || changed;
    assert(!(changed && check && shallowOperation(node, context)));
  }
  return changed;
}

bool Tree::ApplyShallowBottomUp(Tree* t, ShallowOperation shallowOperation,
                                void* context, bool check) {
  bool changed = false;
  for (Tree* child : t->children()) {
    changed |= ApplyShallowBottomUp(child, shallowOperation, context, check);
  }
  changed = shallowOperation(t, context) || changed;
  assert(!(changed && check && shallowOperation(t, context)));
  return changed;
}

bool Tree::deepReplaceWith(const Tree* target, const Tree* replacement) {
  assert(SharedTreeStack->isAfter(replacement, this) &&
         SharedTreeStack->isAfter(target, this));
  if (replaceWith(target, replacement)) {
    return true;
  }
  bool changed = false;
  for (Tree* child : children()) {
    changed = child->deepReplaceWith(target, replacement) || changed;
  }
  return changed;
}

bool Tree::deepReplaceWith(const Tree* target, TreeRef& replacement) {
  assert(SharedTreeStack->isAfter(target, this));
  if (replaceWith(target, replacement)) {
    return true;
  }
#if ASSERTIONS
  int index = 0;
#endif
  bool changed = false;
  for (Tree* childTree : children()) {
    bool childChanged = childTree->deepReplaceWith(target, replacement);
#if ASSERTIONS
    if (childChanged && isParametric()) {
      // Variable should not be altered
      assert(index != Parametric::k_variableIndex);
      // The variable should not be in the replacement expression
      if (index == Parametric::FunctionIndex(this)) {
        /* Only one of these two assertions could be checked depending on if the
         * expression has been projected or not. Test both for now. */
        // Projected expression :
        assert(!replacement->hasDescendantSatisfying(
            [](const Tree* t) { return t->isVar(); }));
        // Unprojected expressions :
        const Tree* variable = child(Parametric::k_variableIndex);
        for (const Tree* t : replacement->selfAndDescendants()) {
          assert(!t->treeIsIdenticalTo(variable));
        }
      }
    }
    index++;
#endif
    changed = childChanged || changed;
  }
  return changed;
}

bool Tree::replaceWith(const Tree* target, const Tree* replacement) {
  if (treeIsIdenticalTo(target)) {
    cloneTreeOverTree(replacement);
    return true;
  }
  return false;
}

const Tree* Tree::firstDescendantSatisfying(Predicate predicate) const {
  for (const Tree* d : selfAndDescendants()) {
    if (predicate(d)) {
      return d;
    }
  }
  return nullptr;
}

const Tree* Tree::firstChildSatisfying(Predicate predicate) const {
  for (const Tree* d : children()) {
    if (predicate(d)) {
      return d;
    }
  }
  return nullptr;
}

Tree* Tree::cloneTree() const { return SharedTreeStack->clone(this); }
Tree* Tree::cloneNode() const { return SharedTreeStack->clone(this, false); }

// Tree edition

Tree* Tree::cloneAt(const Tree* nodeToClone, bool before, bool newIsTree,
                    bool at) {
  assert(nodeToClone);
  Tree* destination = before ? this : nextNode();
  size_t size = newIsTree ? nodeToClone->treeSize() : nodeToClone->nodeSize();
  SharedTreeStack->insertBlocks(destination->block(), nodeToClone->block(),
                                size, at);
#if POINCARE_TREE_STACK_VISUALIZATION
  Log("Insert", destination->block(), size);
#endif
  return destination;
}

Tree* Tree::moveAt(Tree* nodeToMove, bool before, bool newIsTree, bool at) {
  assert(nodeToMove);
  Tree* destination = before ? this : nextNode();
  size_t size = newIsTree ? nodeToMove->treeSize() : nodeToMove->nodeSize();
  assert(SharedTreeStack->contains(nodeToMove->block()));
  SharedTreeStack->moveBlocks(destination->block(), nodeToMove->block(), size,
                              at);
  Block* dst = destination->block();
  Block* addedBlock = dst > nodeToMove->block() ? dst - size : dst;
#if POINCARE_TREE_STACK_VISUALIZATION
  Log("Insert", addedBlock, size, nodeToMove->block());
#endif
  return Tree::FromBlocks(addedBlock);
}

Tree* Tree::cloneOver(const Tree* newNode, bool oldIsTree, bool newIsTree) {
  assert(newNode);
  Tree* oldNode = this;
  int oldSize = oldIsTree ? oldNode->treeSize() : oldNode->nodeSize();
  int newSize = newIsTree ? newNode->treeSize() : newNode->nodeSize();
  Block* oldBlock = oldNode->block();
  const Block* newBlock = newNode->block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return Tree::FromBlocks(oldBlock);
  }
  /* This assert prevents cloning a parent onto a child, indeed this could lead
   * to a malformed tree, for example:
   * | Abs | One |
   *   new   old
   * Should lead to | Abs | Abs | One | but indeed leads to | Abs | Abs | Abs |
   * The reverse (cloning a child onto a parent) is allowed and used
   */
  assert(!(newBlock < oldBlock && oldBlock < newBlock + newSize));
  size_t minSize = std::min(oldSize, newSize);
  SharedTreeStack->replaceBlocks(oldBlock, newBlock, minSize);
  if (oldSize > newSize) {
    SharedTreeStack->removeBlocks(oldBlock + minSize, oldSize - newSize);
  } else {
    SharedTreeStack->insertBlocks(oldBlock + minSize, newBlock + minSize,
                                  newSize - oldSize);
  }
#if POINCARE_TREE_STACK_VISUALIZATION
  Log("Replace", oldBlock, newSize);
#endif
  return Tree::FromBlocks(oldBlock);
}

Tree* Tree::moveOver(Tree* newNode, bool oldIsTree, bool newIsTree) {
  assert(newNode);
  Tree* oldNode = this;
  int oldSize = oldIsTree ? oldNode->treeSize() : oldNode->nodeSize();
  int newSize = newIsTree ? newNode->treeSize() : newNode->nodeSize();
  Block* oldBlock = oldNode->block();
  Block* newBlock = newNode->block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return Tree::FromBlocks(oldBlock);
  }
  Block* finalBlock = oldBlock;
  assert(SharedTreeStack->contains(newNode->block()));
  // Fractal scheme
  assert(!(newIsTree && oldNode->hasAncestor(newNode, true)));
  if (oldIsTree && newNode->hasAncestor(oldNode, true)) {
    oldSize -= newSize;
  }
  // Move newNode at oldNode to preserve TreeRefs on oldNode's root.
  SharedTreeStack->moveBlocks(oldBlock, newBlock, newSize, true);
  if (oldBlock > newBlock) {
    finalBlock -= newSize;
  }
  SharedTreeStack->removeBlocks(finalBlock + newSize, oldSize);
#if POINCARE_TREE_STACK_VISUALIZATION
  if (oldBlock < newBlock) {
    newBlock -= oldSize;
  }
  Log("Replace", finalBlock, newSize, newBlock);
#endif
  return Tree::FromBlocks(finalBlock);
}

void Tree::remove(bool isTree) {
  Block* b = block();
  size_t size = isTree ? treeSize() : nodeSize();
  SharedTreeStack->removeBlocks(b, size);
#if POINCARE_TREE_STACK_VISUALIZATION
  Log("Remove", nullptr, INT_MAX, b);
#endif
}

Tree* Tree::detach(bool isTree) {
  Block* destination = SharedTreeStack->lastBlock();
  size_t sizeToMove = isTree ? treeSize() : nodeSize();
  Block* source = block();
  SharedTreeStack->moveBlocks(destination, source, sizeToMove, true);
#if POINCARE_TREE_STACK_VISUALIZATION
  Log("Detach", destination - sizeToMove, sizeToMove, source);
#endif
  return Tree::FromBlocks(destination - sizeToMove);
}

void Tree::swapWithTree(Tree* t) {
  if (block() > t->block()) {
    return t->swapWithTree(this);
  }
  t->moveTreeBeforeNode(this);
  moveTreeBeforeNode(t);
}

void SwapTreesPointers(Tree** t1, Tree** t2) {
  TreeRef t1Ref(*t1);
  TreeRef t2Ref(*t2);
  (*t1)->swapWithTree(*t2);
  *t1 = t1Ref;
  *t2 = t2Ref;
}

int NumberOfNextTreeTo(const Tree* from, const Tree* to) {
  int i = 0;
  while (from < to) {
    from = from->nextTree();
    i++;
  }
  assert(from == to);
  return i;
}

}  // namespace Poincare::Internal
