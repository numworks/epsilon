#include "tree_block.h"
#include "handle.h"
#include <assert.h>

namespace Poincare {

/* TypeTreeBlock */

#if POINCARE_TREE_LOG
void TypeTreeBlock::log(std::ostream & stream, bool recursive, int indentation, bool verbose) {
  stream << "\n";
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
  stream << "<";
  logNodeName(stream);
  if (verbose) {
    stream << " size=\"" << nodeSize() << "\"";
    stream << " address=\"" << this << "\"";
  }
  logAttributes(stream);
  bool tagIsClosed = false;
  if (recursive) {
    for (IndexedTypeTreeBlock child : directChildren()) {
      if (!tagIsClosed) {
        stream << ">";
        tagIsClosed = true;
      }
      child.m_block->log(stream, recursive, indentation + 1, verbose);
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

const TypeTreeBlock * TypeTreeBlock::nextNode() const {
  return this + nodeSize();
}

void TypeTreeBlock::copyTo(TreeBlock * address) {
  memcpy(address, this, treeSize());
}

const TypeTreeBlock * TypeTreeBlock::previousNode(const TreeBlock * firstBlock) const {
  if (this == firstBlock) {
    return nullptr;
  }
  const TypeTreeBlock * block = static_cast<const TypeTreeBlock *>(previousBlock());
  return this - block->nodeSize(false);
}

const TypeTreeBlock * TypeTreeBlock::nextSibling() const {
  const TypeTreeBlock * result = this;
  int nbOfChildrenToScan = result->numberOfChildren();
  while (nbOfChildrenToScan > 0) {
    result = result->nextNode();
    nbOfChildrenToScan += result->numberOfChildren() - 1;
  }
  return result->nextNode();
}

void TypeTreeBlock::recursivelyApply(InPlaceTreeFunction treeFunction) {
  for (IndexedTypeTreeBlock indexedChild : directChildren()) {
    indexedChild.m_block->recursivelyApply(treeFunction);
  }
  (this->*treeFunction)();
}

const TypeTreeBlock * TypeTreeBlock::previousRelative(const TreeBlock * firstBlock, bool parent) const {
  const TypeTreeBlock * currentNode = this;
  const TypeTreeBlock * closestSibling = nullptr;
  int nbOfChildrenToScan = 1;
  do {
    currentNode = currentNode->previousNode(firstBlock);
    if (currentNode == nullptr) {
      return nullptr;
    }
    nbOfChildrenToScan += currentNode->numberOfChildren() - 1;
    if (nbOfChildrenToScan == 0) {
      closestSibling = currentNode;
    }
  } while (nbOfChildrenToScan <= 0);
  return parent ? currentNode : closestSibling;
}

const TypeTreeBlock * TypeTreeBlock::previousSibling(const TreeBlock * firstBlock) const {
  return previousRelative(firstBlock, false);
}

const TypeTreeBlock * TypeTreeBlock::parent(const TreeBlock * firstBlock) const {
  return previousRelative(firstBlock, true);
}

const TypeTreeBlock * TypeTreeBlock::root(const TreeBlock * firstBlock) const {
  const TypeTreeBlock * ancestor = this;
  do {
    ancestor = ancestor->parent(firstBlock);
  } while (ancestor != nullptr);
  return ancestor;
}

int TypeTreeBlock::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  const TypeTreeBlock * nextSiblingNode = nextSibling();
  const TypeTreeBlock * currentNode = nextNode();
  while (currentNode != nextSiblingNode) {
    result++;
    currentNode = currentNode->nextNode();
  }
  return result;
}

const TypeTreeBlock * TypeTreeBlock::childAtIndex(int i) const {
  for (IndexedTypeTreeBlock indexedChild : directChildren()) {
    if (indexedChild.m_index == i) {
      return indexedChild.m_block;
    }
  }
  return nullptr;
}

int TypeTreeBlock::indexOfChild(const TypeTreeBlock * child) const {
  assert(child != nullptr);
  int childrenCount = numberOfChildren();
  const TypeTreeBlock * childAtIndexi = nextNode();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndexi == child) {
      return i;
    }
    childAtIndexi = childAtIndexi->nextNode();
  }
  return -1;
}

int TypeTreeBlock::indexInParent(const TreeBlock * firstBlock) const {
  const TypeTreeBlock * p = parent(firstBlock);
  if (p == nullptr) {
    return -1;
  }
  return p->indexOfChild(this);
}

bool TypeTreeBlock::hasChild(const TypeTreeBlock * child) const {
  for (IndexedTypeTreeBlock indexedChild : directChildren()) {
    if (child == indexedChild.m_block) {
      return true;
    }
  }
  return false;
}

bool TypeTreeBlock::hasAncestor(const TreeBlock * firstBlock, const TypeTreeBlock * block, bool includeSelf) const {
  const TypeTreeBlock * ancestor = this;
  do {
    if (ancestor == block) {
      return includeSelf || (ancestor != this);
    }
    ancestor = ancestor->parent(firstBlock);
  } while (ancestor != nullptr);
  return false;
}

bool TypeTreeBlock::hasSibling(const TreeBlock * firstBlock, const TypeTreeBlock * sibling) const {
  const TypeTreeBlock * p = parent(firstBlock);
  if (p == nullptr) {
    return false;
  }
  for (IndexedTypeTreeBlock indexedBlock : p->directChildren()) {
    if (indexedBlock.m_block == sibling) {
      return true;
    }
  }
  return false;
}

TypeTreeBlock::BackwardsDirect::Iterator::Memoizer::Memoizer(TypeTreeBlock * treeBlock) :
  m_block(treeBlock),
  m_firstMemoizedSubtreeIndex(0),
  m_firstSubtreeIndex(0),
  m_numberOfChildren(treeBlock->numberOfChildren())
{
  memoizeUntilIndex(m_numberOfChildren);
}

TypeTreeBlock * TypeTreeBlock::BackwardsDirect::Iterator::Memoizer::childAtIndex(int i) {
  if (i < m_firstSubtreeIndex || i >= m_firstSubtreeIndex + m_numberOfChildren) {
    memoizeUntilIndex(i + 1);
  }
  assert(i >= m_firstSubtreeIndex && i < m_firstSubtreeIndex + m_numberOfChildren);
  return m_children[(m_firstMemoizedSubtreeIndex + i - m_firstSubtreeIndex) % k_maxNumberOfMemoizedSubtrees];
}

void TypeTreeBlock::BackwardsDirect::Iterator::Memoizer::memoizeUntilIndex(int i) {
  for (IndexedTypeTreeBlock indexedBlock : m_block->directChildren()) {
    m_children[indexedBlock.m_index % k_maxNumberOfMemoizedSubtrees] = indexedBlock.m_block;
    if (indexedBlock.m_index + 1 == i) {
      break;
    }
  }
  if (i < k_maxNumberOfMemoizedSubtrees) {
    m_firstMemoizedSubtreeIndex = 0;
  } else {
    m_firstMemoizedSubtreeIndex = std::min(i, m_block->numberOfChildren()) % k_maxNumberOfMemoizedSubtrees;
  }
}

}
