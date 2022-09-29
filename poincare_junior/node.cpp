#include "node.h"
#include "cache_pool.h"

namespace Poincare {

#if POINCARE_TREE_LOG
void Node::log(std::ostream & stream, bool recursive, int indentation, bool verbose) const {
  stream << "\n";
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
  }
  stream << "<";
  logNodeName(stream);
  if (verbose) {
    stream << " size=\"" << nodeSize() << "\"";
    stream << " address=\"" << m_block << "\"";
  }
  logAttributes(stream);
  bool tagIsClosed = false;
  if (recursive) {
    for (NodeIterator::IndexedNode child : NodeIterator(*this).directChildren()) {
      if (!tagIsClosed) {
        stream << ">";
        tagIsClosed = true;
      }
      child.m_node.log(stream, recursive, indentation + 1, verbose);
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

void Node::copyTreeTo(void * address) const {
  memcpy(address, m_block, treeSize());
}

const Node Node::previousNode() const {
  if (m_block == CachePool::sharedCachePool()->firstBlock()) {
    return Node();
  }
  const Block * block = m_block->previous();
  return Node(m_block - Node(block).nodeSize(false));
}

const Node Node::previousTree() const {
  return previousRelative(false);
}

const Node Node::parent() const {
  return previousRelative(true);
}

const Node Node::root() const {
  Node ancestor = *this;
  do {
    ancestor = ancestor.parent();
  } while (ancestor != Node());
  return ancestor;
}

int Node::numberOfDescendants(bool includeSelf) const {
  int result = includeSelf ? 1 : 0;
  Node nextTreeNode = nextTree();
  Node currentNode = nextNode();
  while (currentNode != nextTreeNode) {
    result++;
    currentNode = currentNode.nextNode();
  }
  return result;
}

const Node Node::childAtIndex(int i) const {
  for (NodeIterator::IndexedNode indexedChild : NodeIterator(*this).directChildren()) {
    if (indexedChild.m_index == i) {
      return indexedChild.m_node;
    }
  }
  return Node();
}

int Node::indexOfChild(const Node child) const {
  assert(child.m_block != nullptr);
  int childrenCount = numberOfChildren();
  Node childAtIndexi = nextNode();
  for (int i = 0; i < childrenCount; i++) {
    if (childAtIndexi == child) {
      return i;
    }
    childAtIndexi = childAtIndexi.nextNode();
  }
  return -1;
}

int Node::indexInParent() const {
  const Node p = parent();
  if (p == Node()) {
    return -1;
  }
  return p.indexOfChild(*this);
}

bool Node::hasChild(const Node child) const {
  for (NodeIterator::IndexedNode indexedChild : NodeIterator(*this).directChildren()) {
    if (child == indexedChild.m_node) {
      return true;
    }
  }
  return false;
}

bool Node::hasAncestor(const Node node, bool includeSelf) const {
  Node ancestor = *this;
  do {
    if (ancestor == node) {
      return includeSelf || (ancestor != *this);
    }
    ancestor = ancestor.parent();
  } while (ancestor != Node());
  return false;
}

bool Node::hasSibling(const Node sibling) const {
  const Node p = parent();
  if (p == Node()) {
    return false;
  }
  for (NodeIterator::IndexedNode indexedChild : NodeIterator(p).directChildren()) {
    if (indexedChild.m_node == sibling) {
      return true;
    }
  }
  return false;
}

void Node::recursivelyApply(InPlaceTreeFunction treeFunction) {
  for (NodeIterator::IndexedNode indexedChild : NodeIterator(*this).directChildren()) {
    indexedChild.m_node.recursivelyApply(treeFunction);
  }
  (*treeFunction)(*this);
}

const ExpressionInterface * Node::expressionInterface() const {
  uint8_t typeIndex = static_cast<uint8_t>(*m_block);
  assert(typeIndex >= k_offsetOfExpressionInterfaces && typeIndex < k_offsetOfLayoutInterfaces);
  typeIndex -= k_offsetOfExpressionInterfaces;
  size_t numberOfInternalExpressions = sizeof(k_internalExpressionInterfaces) / sizeof(const InternalExpressionInterface *);
  if (typeIndex < numberOfInternalExpressions) {
    return k_internalExpressionInterfaces[typeIndex];
  }
  typeIndex -= numberOfInternalExpressions;
  assert(typeIndex < sizeof(k_expressionInterfaces) / sizeof(const ExpressionInterface *));
  return k_expressionInterfaces[typeIndex];
}

const Node Node::previousRelative(bool parent) const {
  Node currentNode = *this;
  Node closestSibling;
  int nbOfChildrenToScan = 1;
  do {
    currentNode = currentNode.previousNode();
    if (currentNode == Node()) {
      return Node();
    }
    nbOfChildrenToScan += currentNode.numberOfChildren() - 1;
    if (nbOfChildrenToScan == 0) {
      closestSibling = currentNode;
    }
  } while (nbOfChildrenToScan <= 0);
  return parent ? currentNode : closestSibling;
}

// Iterator

NodeIterator::BackwardsDirect::Iterator::Memoizer::Memoizer(Node node) :
  m_node(node),
  m_firstMemoizedSubtreeIndex(0),
  m_firstSubtreeIndex(0),
  m_numberOfChildren(node.numberOfChildren())
{
  memoizeUntilIndex(m_numberOfChildren);
}

NodeIterator::IndexedNode NodeIterator::BackwardsDirect::Iterator::Memoizer::childAtIndex(int i) {
  if (i < m_firstSubtreeIndex || i >= m_firstSubtreeIndex + m_numberOfChildren) {
    memoizeUntilIndex(i + 1);
  }
  assert(i >= m_firstSubtreeIndex && i < m_firstSubtreeIndex + m_numberOfChildren);
  return m_children[(m_firstMemoizedSubtreeIndex + i - m_firstSubtreeIndex) % k_maxNumberOfMemoizedSubtrees];
}

void NodeIterator::BackwardsDirect::Iterator::Memoizer::memoizeUntilIndex(int i) {
  for (NodeIterator::IndexedNode indexedChild : NodeIterator(m_node).directChildren()) {
    m_children[indexedChild.m_index % k_maxNumberOfMemoizedSubtrees] = indexedChild;
    if (indexedChild.m_index + 1 == i) {
      break;
    }
  }
  if (i < k_maxNumberOfMemoizedSubtrees) {
    m_firstMemoizedSubtreeIndex = 0;
  } else {
    m_firstMemoizedSubtreeIndex = std::min(i, m_node.numberOfChildren()) % k_maxNumberOfMemoizedSubtrees;
  }
}

}
