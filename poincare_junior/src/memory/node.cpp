#include "cache_pool.h"
#include "edition_reference.h"
#include "node_iterator.h"
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/symbol.h>

namespace PoincareJ {

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
    for (const auto [child, index] : NodeIterator::Children<Forward, NoEditable>(*this)) {
      if (!tagIsClosed) {
        stream << ">";
        tagIsClosed = true;
      }
      child.log(stream, recursive, indentation + 1, verbose);
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
    "Constant",
    "Addition",
    "Multiplication",
    "Power",
    "Factorial",
    "UserSymbol",
    "UserFunction",
    "UserSequence",
    "Subtraction",
    "Division",
    "Set",
    "List",
    "Polynomial",
    "HorizontalLayout"
  };
  static_assert(sizeof(names)/sizeof(const char *) == static_cast<uint8_t>(BlockType::NumberOfTypes));
  stream << names[static_cast<uint8_t>(*m_block)];
}

void Node::logAttributes(std::ostream & stream) const {
  if (block()->isNAry()) {
    stream << " numberOfChildren=\"" << numberOfChildren() << "\"";
    if (type() == BlockType::Polynomial) {
      for (int i = 0; i < Polynomial::NumberOfTerms(*this); i++) {
        stream << " exponent(\"" << i << "\") = \"" << static_cast<int>(static_cast<uint8_t>(*(block()->nextNth(2 + i)))) << "\"";
      }
    }
    return;
  }
  if (block()->isNumber() || type() == BlockType::Constant) {
    stream << " value=\"" << Approximation::To<float>(*this) << "\"";
    return;
  }
  if (block()->isUserNamed()) {
    char buffer[64];
    Symbol::GetName(*this, buffer, sizeof(buffer));
    stream << " value=\"" << buffer << "\"";
  }
}

#endif

void Node::copyTreeTo(void * address) const {
  memcpy(address, m_block, treeSize());
}

const Node Node::previousNode() const {
  CachePool * cache = CachePool::sharedCachePool();
  if (m_block == cache->firstBlock() || (cache->size() == 0 && m_block == cache->lastBlock())) {
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
  while (ancestor.parent() != Node()) {
    ancestor = ancestor.parent();
  }
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
  for (const auto [child, index] : NodeIterator::Children<Forward, NoEditable>(*this)) {
    if (index == i) {
      return child;
    }
  }
  return Node();
}

int Node::indexOfChild(const Node child) const {
  assert(child.m_block != nullptr);
  for (const std::pair<Node, int> indexedChild : NodeIterator::Children<Forward, NoEditable>(*this)) {
    if (child == std::get<Node>(indexedChild)) {
      return std::get<int>(indexedChild);
    }
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
  return indexOfChild(child) >= 0;
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
  for (const auto& [child, index]: NodeIterator::Children<Forward, NoEditable>(p)) {
    if (child == sibling) {
      return true;
    }
  }
  return false;
}

void Node::recursivelyGet(InPlaceConstTreeFunction treeFunction) const {
  for (const auto& [child, index] : NodeIterator::Children<Forward, NoEditable>(*this)) {
    child.recursivelyGet(treeFunction);
  }
  (*treeFunction)(*this);
}

const Node Node::previousRelative(bool parent) const {
  Node currentNode = *this;
  Node closestSibling;
  int nbOfChildrenToScan = 0;
  do {
    currentNode = currentNode.previousNode();
    if (currentNode.isUninitialized()) {
      break;
    }
    nbOfChildrenToScan += currentNode.numberOfChildren() - 1;
    if (nbOfChildrenToScan == -1) {
      closestSibling = currentNode;
    }
  } while (nbOfChildrenToScan < 0);
  return parent ? currentNode : closestSibling;
}

}
