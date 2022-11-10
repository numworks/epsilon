#include "cache_pool.h"
#include "edition_reference.h"
#include "node.h"
#include "node_iterator.h"

namespace Poincare {

template <typename T, typename... Types>
Node Node::Push(Types... args) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock * newNode = static_cast<TypeBlock *>(pool->lastBlock());
  Block block;
  size_t i = 0;
  bool endOfNode = false;
  do {
    endOfNode = T::CreateBlockAtIndex(&block, i++, args...);
    pool->pushBlock(block);
  } while (!endOfNode);
  return Node(newNode);
}

#if POINCARE_MEMORY_TREE_LOG
void Node::log(std::ostream & stream, bool recursive, int indentation, bool verbose) const {
  stream << "\n";
  for (int i = 0; i < indentation; ++i) {
    stream << "  ";
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
    for (const NodeIterator::IndexedNode child : NodeIterator(*this).forwardConstChildren()) {
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
    logName(stream);
    stream << ">";
  } else {
    stream << "/>";
  }
}

void Node::logName(std::ostream & stream) const {
  constexpr const char * names[] = {
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
    "Addition",
    "Multiplication",
    "Power",
    "Constant",
    "Subtraction",
    "Division",
    "HorizontalLayout"
  };
  static_assert(sizeof(names)/sizeof(const char *) == static_cast<uint8_t>(BlockType::NumberOfTypes));
  stream << names[static_cast<uint8_t>(*m_block)];
}

void Node::logAttributes(std::ostream & stream) const {
  switch (type()) {
    case BlockType::Addition:
    case BlockType::Multiplication:
      stream << " numberOfChildren=\"" << numberOfChildren() << "\"";
      return;
    case BlockType::IntegerShort:
    case BlockType::IntegerPosBig:
    case BlockType::IntegerNegBig:
    case BlockType::RationalShort:
    case BlockType::RationalPosBig:
    case BlockType::RationalNegBig:
    case BlockType::Float:
      stream << " value=\"" << EExpression::Approximate(m_block) << "\"";
      return;
    case BlockType::Constant:
      stream << " value=\"" << Constant::Value(m_block) << "\"";
      return;
    default:
      return;
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
  for (const NodeIterator::IndexedNode indexedChild : NodeIterator(*this).forwardConstChildren()) {
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
  for (const NodeIterator::IndexedNode indexedChild : NodeIterator(*this).forwardConstChildren()) {
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
  for (const NodeIterator::IndexedNode indexedChild : NodeIterator(p).forwardConstChildren()) {
    if (indexedChild.m_node == sibling) {
      return true;
    }
  }
  return false;
}

void Node::recursivelyGet(InPlaceConstTreeFunction treeFunction) const {
  for (const NodeIterator::IndexedNode child : NodeIterator(*this).forwardConstChildren()) {
    child.m_node.recursivelyGet(treeFunction);
  }
  (*treeFunction)(*this);
}

void Node::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (NodeIterator::IndexedNode child : NodeIterator(*this).forwardEditableChildren()) {
    child.m_node.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
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

}

template Poincare::Node Poincare::Node::Push<Poincare::Addition, int>(int);
template Poincare::Node Poincare::Node::Push<Poincare::Division>();
template Poincare::Node Poincare::Node::Push<Poincare::Multiplication, int>(int);
template Poincare::Node Poincare::Node::Push<Poincare::Power>();
template Poincare::Node Poincare::Node::Push<Poincare::Subtraction>();
template Poincare::Node Poincare::Node::Push<Poincare::Constant, char16_t>(char16_t);
template Poincare::Node Poincare::Node::Push<Poincare::IntegerShort, int>(int);
