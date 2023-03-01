#include "node_iterator.h"
#include <poincare_junior/src/expression/approximation.h>
#include <poincare_junior/src/expression/polynomial.h>
#include <poincare_junior/src/expression/symbol.h>
#include <poincare_junior/src/layout/code_point_layout.h>

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
    "RackLayout",
    "FractionLayout",
    "ParenthesisLayout",
    "VerticalOffsetLayout",
    "CodePointLayout",
    "NodeBorder",
    "Placeholder",
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
  if (block()->isUserNamed() || type() == BlockType::CodePointLayout) {
    char buffer[64];
    (block()->isUserNamed() ? Symbol::GetName : CodePointLayout::GetName)(*this, buffer, sizeof(buffer));
    stream << " value=\"" << buffer << "\"";
  }
}

void Node::logBlocks(std::ostream & stream, bool recursive, int indentation) const {
  for (int i = 0; i < indentation; ++i) {
      stream << "  ";
  }
  stream << "[";
  logName(stream);
  stream << "]";
  int size = nodeSize();
  if (size > 1) {
    for (int i = 1; i < size - 1; i++) {
      stream << "[" << static_cast<int>(static_cast<uint8_t>(m_block[i])) << "]";
    }
    stream << "[";
    logName(stream);
    stream << "]";
  }
  stream << "\n";
  if (recursive) {
    indentation += 1;
    for (const auto [child, index] : NodeIterator::Children<Forward, NoEditable>(*this)) {
      child.logBlocks(stream, recursive, indentation);
    }
  }
}

#endif

void Node::copyTreeTo(void * address) const {
  memcpy(address, m_block, treeSize());
}

const Node Node::previousNode() const {
  if (type() == BlockType::NodeBorder) {
    return Node();
  }
  int previousSize = static_cast<TypeBlock *>(m_block->previous())->nodeSize(false);
  return Node(m_block - previousSize);
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
  for (const auto [c, index] : NodeIterator::Children<Forward, NoEditable>(*this)) {
    if (child == c) {
      return index;
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
