#include "pattern_matching.h"

using namespace PoincareJ;

bool PatternMatching::Context::isUninitialized() const {
  for (const Node &node : m_array) {
    if (!node.isUninitialized()) {
      return false;
    }
  }
  return true;
}

#if POINCARE_MEMORY_TREE_LOG
void PatternMatching::Context::log() const {
  std::cout << "<Context>";
  for (const Node &node : m_array) {
    node.log(std::cout, true, 1);
  }
  std::cout << "\n</Context>";
}
#endif

PatternMatching::Context PatternMatching::Match(const Node pattern,
                                                const Node source,
                                                Context result) {
  Pool::Nodes patternNodes = Pool::Nodes(
      pattern.block(), pattern.nextTree().block() - pattern.block());
  Node currentNode = source;
  for (const Node node : patternNodes) {
    if (node.type() == BlockType::Placeholder) {
      Placeholder::Tag tag = Placeholder::NodeToTag(node);
      if (result[tag].isUninitialized()) {
        result[tag] = currentNode;
      } else if (!result[tag].treeIsIdenticalTo(currentNode)) {
        return Context();
      }
      currentNode = currentNode.nextTree();
    } else {
      if (!node.isIdenticalTo(currentNode)) {
        return Context();
      }
      currentNode = currentNode.nextNode();
    }
  }
  return result;
}

EditionReference PatternMatching::Create(const Node structure,
                                         const Context context) {
  EditionPool *editionPool = EditionPool::sharedEditionPool();
  Node top = editionPool->lastBlock();
  // TODO introduce a DFS iterator in node_iterator and use it here
  Pool::Nodes nodes = Pool::Nodes(
      structure.block(), structure.nextTree().block() - structure.block());
  for (const Node node : nodes) {
    if (node.type() != BlockType::Placeholder) {
      editionPool->clone(node, false);
      continue;
    }
    Placeholder::Tag tag = Placeholder::NodeToTag(node);
    Node nodeToInsert = context[tag];
    assert(!nodeToInsert.isUninitialized());
    editionPool->clone(nodeToInsert, true);
  }
  return EditionReference(top);
}
