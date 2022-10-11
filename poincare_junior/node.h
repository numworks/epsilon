#ifndef POINCARE_NODE_H
#define POINCARE_NODE_H

#include "interfaces/interfaces.h"

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
  constexpr Node(TypeBlock * block = nullptr) : m_block(block) {}
  Node(const Block * block) : m_block(static_cast<TypeBlock *>(const_cast<Block *>(block))) {}

  bool operator==(const Node& n) const { return n.m_block == m_block; }
  bool operator!=(const Node& n) { return n.m_block != m_block; }

#if POINCARE_TREE_LOG
  void log(std::ostream & stream, bool recursive = true, int indentation = 0, bool verbose = true) const;
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

  constexpr size_t nodeSize(bool head = true) const { return interface()->nodeSize(block(), head); }
  constexpr int numberOfChildren() const { return interface()->numberOfChildren(block()); }

  // Virtuality
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const { return interface()->logNodeName(stream); }
  void logAttributes(std::ostream & stream) const { return interface()->logAttributes(m_block, stream); }
#endif

  constexpr BlockType type() const { return m_block->type(); }

  // Recursive helper
  typedef void (*InPlaceTreeFunction)(Node node);
  void recursivelyApply(InPlaceTreeFunction treeFunction);

  const ExpressionInterface * expressionInterface() const;

private:
  const Node previousRelative(bool parent) const;
  constexpr const Interface * interface() const { return k_interfaces[static_cast<uint8_t>(*block())]; }

  TypeBlock * m_block;
};

class NodeIterator {
public:
  NodeIterator(const Node n) : m_node(n) {}

  struct IndexedNode {
    const Node m_node;
    int m_index;
  };

  class Direct {
  public:
    Direct(const Node node) : m_node(node) {}
    class Iterator {
    public:
      Iterator(const Node node, int index) : m_indexedNode({.m_node = node, .m_index = index}) {}
      IndexedNode operator*() { return m_indexedNode; }
      bool operator!=(const Iterator& it) const { return (m_indexedNode.m_index != it.m_indexedNode.m_index); }
    protected:
      IndexedNode m_indexedNode;
    };
  protected:
    const Node m_node;
  };

  class ForwardDirect final : public Direct {
  public:
    using Direct::Direct;
    class Iterator : public Direct::Iterator {
    public:
      using Direct::Iterator::Iterator;
      Iterator operator++() {
        return Iterator(m_indexedNode.m_node.nextTree(), m_indexedNode.m_index + 1);
      }
    };
    Iterator begin() const { return Iterator(m_node.nextNode(), 0); }
    Iterator end() const { return Iterator(Node(), m_node.numberOfChildren()); }
  };

  class BackwardsDirect final : public Direct {
  public:
    using Direct::Direct;
    class Iterator : public Direct::Iterator {
    public:
      using Direct::Iterator::Iterator;
      Iterator operator++() {
        return Iterator(m_indexedNode.m_node.previousTree(), m_indexedNode.m_index - 1);
      }
    };
    Iterator begin() const { return Iterator(m_node.nextTree().previousNode(), m_node.numberOfChildren() - 1); }
    Iterator end() const { return Iterator(Node(), -1); }
  };

  ForwardDirect directChildren() { return ForwardDirect(m_node); }
  BackwardsDirect backwardsDirectChildren() { return BackwardsDirect(m_node); }

private:
  const Node m_node;
};



}

#endif
