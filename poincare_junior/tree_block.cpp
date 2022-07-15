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
  Handle * h = Handle::CreateHandle(this);
  h->logNodeName(stream);
  if (verbose) {
    stream << " size=\"" << h->nodeSize() << "\"";
  }
  h->logAttributes(stream);
  bool tagIsClosed = false;
  if (recursive) {
    for (TypeTreeBlock * child : directChildren()) {
      if (!tagIsClosed) {
        stream << ">";
        tagIsClosed = true;
      }
      child->log(stream, recursive, indentation + 1, verbose);
    }
  }
  if (tagIsClosed) {
    h = Handle::CreateHandle(this);
    stream << "\n";
    for (int i = 0; i < indentation; ++i) {
      stream << "  ";
    }
    stream << "</";
    h->logNodeName(stream);
    stream << ">";
  } else {
    stream << "/>";
  }
}
#endif

TypeTreeBlock * TypeTreeBlock::nextNode() {
  return this + Handle::CreateHandle(this)->nodeSize();
}

TypeTreeBlock * TypeTreeBlock::previousNode(const TreeBlock * firstBlock) {
  if (this == firstBlock) {
    return nullptr;
  }
  TypeTreeBlock * block = static_cast<TypeTreeBlock *>(previousBlock());
  return this - Handle::CreateHandle(block)->nodeSize();
}

TypeTreeBlock * TypeTreeBlock::nextSibling() {
  TypeTreeBlock * result = this;
  int nbOfChildrenToScan = result->numberOfChildren();
  while (nbOfChildrenToScan > 0) {
    result = result->nextNode();
    nbOfChildrenToScan += result->numberOfChildren() - 1;
  }
  return result->nextNode();
}

TypeTreeBlock * TypeTreeBlock::previousRelative(const TreeBlock * firstBlock, bool parent) {
  TypeTreeBlock * currentNode = this;
  TypeTreeBlock * closestSibling = nullptr;
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

TypeTreeBlock * TypeTreeBlock::previousSibling(const TreeBlock * firstBlock) {
  return previousRelative(firstBlock, false);
}

TypeTreeBlock * TypeTreeBlock::parent(const TreeBlock * firstBlock) {
  return previousRelative(firstBlock, true);
}

TypeTreeBlock * TypeTreeBlock::root(const TreeBlock * firstBlock) {
  TypeTreeBlock * ancestor = this;
  do {
    ancestor = ancestor->parent(firstBlock);
  } while (ancestor != nullptr);
  return ancestor;
}

int TypeTreeBlock::numberOfChildren() const {
  return Handle::CreateHandle(this)->numberOfChildren();
}

int TypeTreeBlock::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  TypeTreeBlock * nextSiblingNode = const_cast<TypeTreeBlock *>(this)->nextSibling();
  TypeTreeBlock * currentNode = const_cast<TypeTreeBlock *>(this)->nextNode();
  while (currentNode != nextSiblingNode) {
    result++;
    currentNode = currentNode->nextNode();
  }
  return result;
}

TypeTreeBlock * TypeTreeBlock::childAtIndex(int i) const {
  int currentChildIndex = 0;
  for (TypeTreeBlock * c : directChildren()) {
    if (currentChildIndex++ == i) {
      return c;
    }
  }
  return nullptr;
}

int TypeTreeBlock::indexOfChild(const TypeTreeBlock * child) const {
  assert(child != nullptr);
  int childrenCount = numberOfChildren();
  TypeTreeBlock * childAtIndexi = const_cast<TypeTreeBlock *>(this)->nextNode();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndexi == child) {
      return i;
    }
    childAtIndexi = childAtIndexi->nextNode();
  }
  return -1;
}

int TypeTreeBlock::indexInParent(const TreeBlock * firstBlock) const {
  TypeTreeBlock * p = const_cast<TypeTreeBlock *>(this)->parent(firstBlock);
  if (p == nullptr) {
    return -1;
  }
  return p->indexOfChild(this);
}

bool TypeTreeBlock::hasChild(const TypeTreeBlock * child) const {
  for (TypeTreeBlock * c : directChildren()) {
    if (child == c) {
      return true;
    }
  }
  return false;
}

bool TypeTreeBlock::hasAncestor(const TreeBlock * firstBlock, const TypeTreeBlock * block, bool includeSelf) const {
  TypeTreeBlock * ancestor = const_cast<TypeTreeBlock *>(this);
  do {
    if (ancestor == block) {
      return includeSelf || (ancestor != this);
    }
    ancestor = ancestor->parent(firstBlock);
  } while (ancestor != nullptr);
  return false;
}

bool TypeTreeBlock::hasSibling(const TreeBlock * firstBlock, const TypeTreeBlock * sibling) const {
  TypeTreeBlock * p = const_cast<TypeTreeBlock *>(this)->parent(firstBlock);
  if (p == nullptr) {
    return false;
  }
  for (TypeTreeBlock * block : p->directChildren()) {
    if (block == sibling) {
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
  int counter = 0;
  for (TypeTreeBlock * block : m_block->directChildren()) {
    m_children[counter % k_maxNumberOfMemoizedSubtrees] = block;
    counter++;
    if (counter == i) {
      break;
    }
  }
  if (i < k_maxNumberOfMemoizedSubtrees) {
    m_firstMemoizedSubtreeIndex = 0;
  } else {
    m_firstMemoizedSubtreeIndex = counter % k_maxNumberOfMemoizedSubtrees;
  }
}

}
