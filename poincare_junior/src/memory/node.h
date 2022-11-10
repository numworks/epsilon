#ifndef POINCARE_MEMORY_NODE_H
#define POINCARE_MEMORY_NODE_H

#include <poincare_junior/src/expression/expressions.h>

namespace Poincare {

/* A block is a byte-long object containing either a type or some value.
 * Several blocks can form a node, like:
 * [INT][LENGTH][DIGIT0][DIGIT1]...[DIGITN][LENGTH][INT]
 * [ADDITION][LENGTH][ADDITION]
 * A node can also be composed of a single block:
 * [COSINE]
 */

class Node {
public:
  template <typename T, typename... Types>
  static Node Push(Types... args);

  constexpr Node(TypeBlock * block = nullptr) : m_block(block) {}
  Node(const Block * block) : m_block(static_cast<TypeBlock *>(const_cast<Block *>(block))) {}

  bool operator==(const Node& n) const { return n.m_block == m_block; }
  bool operator!=(const Node& n) { return n.m_block != m_block; }

#if POINCARE_MEMORY_TREE_LOG
  void log(std::ostream & stream, bool recursive = true, int indentation = 0, bool verbose = true) const;
  void logName(std::ostream & stream) const;
  void logAttributes(std::ostream & stream) const;
#endif

  constexpr TypeBlock * block() const { return m_block; }
  constexpr TypeBlock * block() { return m_block; }
  bool isUninitialized() const { return m_block == nullptr; }
  void copyTreeTo(void * address) const;

  // Block Navigation
  constexpr const Node nextNode() const { return Node(m_block + nodeSize()); }
  constexpr Node nextNode() { return Utils::DeconstifyObj(&Node::nextNode, this); };
  const Node previousNode() const;
  Node previousNode() { return Utils::DeconstifyObj(&Node::previousNode, this);}
  constexpr const Node nextTree() const {
    Node result = *this;
    int nbOfChildrenToScan = result.numberOfChildren();
    while (nbOfChildrenToScan > 0) {
      result = result.nextNode();
      nbOfChildrenToScan += result.numberOfChildren() - 1;
    }
    return result.nextNode();
  }
  constexpr Node nextTree() { return Utils::DeconstifyObj(&Node::nextTree, this); };
  const Node previousTree() const;
  Node previousTree() { return Utils::DeconstifyObj(&Node::previousTree, this);}

  // Sizes
  constexpr size_t treeSize() const { return nextTree().block() - block(); }

  // Node Hierarchy
  const Node parent() const;
  Node parent() { return Utils::DeconstifyObj(&Node::parent, this);}
  const Node root() const;
  Node root() { return Utils::DeconstifyObj(&Node::root, this);}
  int numberOfDescendants(bool includeSelf) const;
  const Node childAtIndex(int index) const;
  Node childAtIndex(int index) { return Utils::DeconstifyObj(&Node::childAtIndex, this, index);}
  int indexOfChild(const Node child) const;
  int indexInParent() const;
  bool hasChild(const Node child) const;
  bool hasAncestor(const Node node, bool includeSelf) const;
  bool hasSibling(const Node e) const;

  constexpr size_t nodeSize(bool head = true) const {
    // TODO: generate this switch using a C Macro
    switch (type()) {
      case BlockType::IntegerShort:
        return IntegerShort::k_numberOfBlocksInNode;
      case BlockType::IntegerPosBig:
      case BlockType::IntegerNegBig:
      {
        return IntegerBig::k_numberOfMetaBlocksInNode + IntegerBig::NumberOfDigits(block(), head);
      }
      case BlockType::RationalShort:
        return RationalShort::k_numberOfBlocksInNode;
      case BlockType::RationalPosBig:
      case BlockType::RationalNegBig:
      {
        const TypeBlock * b = block();
        if (!head) {
          b = static_cast<const TypeBlock *>(b->previousNth(RationalBig::NumberOfDigitsFromTail(b) + RationalBig::k_numberOfMetaBlocksInNode - 1));
        }
        return RationalBig::k_numberOfMetaBlocksInNode + RationalBig::NumeratorNumberOfDigits(b) + RationalBig::DenominatorNumberOfDigits(b);
      }
      case BlockType::Float:
        return sizeof(float)/sizeof(uint8_t);
      case BlockType::Addition:
      case BlockType::Multiplication:
        return NAry::k_numberOfBlocksInNode;
      case BlockType::Constant:
        return Constant::k_numberOfBlocksInNode;
      default:
        return 1;
    }
  }
  constexpr int numberOfChildren() const {
    // TODO: generate this switch using a C Macro
    switch (type()) {
      case BlockType::Addition:
      case BlockType::Multiplication:
        return static_cast<uint8_t>(*(m_block->next()));
      case BlockType::Power:
      case BlockType::Subtraction:
      case BlockType::Division:
        return 2;
      default:
        return 0;
    }
  }

  constexpr BlockType type() const { return m_block->type(); }

  // Recursive helper
  typedef void (*InPlaceConstTreeFunction)(const Node node);
  void recursivelyGet(InPlaceConstTreeFunction treeFunction) const;
  typedef void (*InPlaceTreeFunction)(Node node);
  void recursivelyEdit(InPlaceTreeFunction treeFunction);

private:
  const Node previousRelative(bool parent) const;
  TypeBlock * m_block;
};

}

#endif
