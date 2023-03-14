#ifndef POINCARE_MEMORY_NODE_H
#define POINCARE_MEMORY_NODE_H

#include <utility>
#include "type_block.h"
#include <string.h>

#if POINCARE_MEMORY_TREE_LOG
#include <iostream>
#endif

namespace PoincareJ {

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
  constexpr Node(const Block * block) : Node(static_cast<TypeBlock *>(const_cast<Block *>(block))) {}

  bool operator==(const Node& n) const { return n.m_block == m_block; }
  bool operator!=(const Node& n) { return n.m_block != m_block; }

  bool treeIsIdenticalTo(const Node &other) const {
    return memcmp(m_block, other.m_block, treeSize()) == 0;
  }

  bool isIdenticalTo(const Node &other) const {
    return memcmp(m_block, other.m_block, nodeSize()) == 0;
  }

#if POINCARE_MEMORY_TREE_LOG
  __attribute__((__used__)) void log() const { return log(std::cout); }
  void log(std::ostream & stream, bool recursive = true, int indentation = 0, bool verbose = true) const;
  void logName(std::ostream & stream) const;
  void logAttributes(std::ostream & stream) const;
  __attribute__((__used__)) void logBlocks() const { logBlocks(std::cout); }
  void logBlocks(std::ostream & stream, bool recursive = true, int indentation = 0) const;
#endif

  constexpr TypeBlock * block() const { return m_block; }
  constexpr TypeBlock * block() { return m_block; }
  bool isUninitialized() const { return m_block == nullptr; }
  void copyTreeTo(void * address) const;

  // Node Navigation
  const Node nextNode() const;
  constexpr Node nextNode() { return Utils::DeconstifyObj(&Node::nextNode, this); };
  /* TODO : If costly, previousNode could be optimized by sprinkling TreeBorders
   *        blocks between every tree of the cache. PreviousNode and nextNode
   *        would only have to check for TreeBorders. */
  const Node previousNode() const;
  constexpr Node previousNode() { return Utils::DeconstifyObj(&Node::previousNode, this);}
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
  constexpr size_t treeSize() const { assert(!isUninitialized()); return nextTree().block() - block(); }

  // Node Hierarchy
  /* TODO : parent, previousBlock and similar methods navigating backward could
   *         be forbidden and deleted, optimizing node size and navigation. */
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
  // TODO : Rename this method isRack
  bool isHorizontal() const { return type() == BlockType::RackLayout; }
  bool isEmpty() const { return isHorizontal() && numberOfChildren() == 0; }

  constexpr BlockType type() const { return m_block->type(); }
  constexpr size_t nodeSize() const { return m_block->nodeSize(true); }
  constexpr int numberOfChildren() const { return m_block->numberOfChildren(true); }

  // Recursive helper
  typedef void (*InPlaceConstTreeFunction)(const Node node);
  void recursivelyGet(InPlaceConstTreeFunction treeFunction) const;

  class Blocks {
  public:
    Blocks(const Node * node) : m_begin(node->block()), m_end(node->block() + node->nodeSize()) {}
    const Block * begin() { return m_begin; }
    const Block * end() { return m_end; }
  private:
    const Block * m_begin;
    const Block * m_end;
  };

  Blocks blocks() const { return Blocks(this); }

private:
  bool canNavigateNext() const;
  bool canNavigatePrevious() const;
  const Node previousRelative(bool parent) const;

  TypeBlock * m_block;
};

}

#endif
