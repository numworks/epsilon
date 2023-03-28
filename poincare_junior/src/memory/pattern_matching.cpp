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
        Placeholder::Filter filter = Placeholder::NodeToFilter(node);
        if (!(filter == Placeholder::Filter::None ||
              (filter == Placeholder::Filter::Addition &&
               currentNode.type() == BlockType::Addition) ||
              (filter == Placeholder::Filter::Multiplication &&
               currentNode.type() == BlockType::Multiplication))) {
          return Context();
        }
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
    Placeholder::Filter filter = Placeholder::NodeToFilter(node);
    Node nodeToInsert = context[tag];
    assert(!nodeToInsert.isUninitialized());
    if (filter == Placeholder::Filter::Others) {
      int childrenToInsert = nodeToInsert.numberOfChildren() - 1;
      bool isAddition = (nodeToInsert.type() == BlockType::Addition);
      if (childrenToInsert > 1) {
        if (isAddition) {
          editionPool->push<BlockType::Addition>(childrenToInsert);
        } else {
          editionPool->push<BlockType::Multiplication>(childrenToInsert);
        }
        nodeToInsert = nodeToInsert.childAtIndex(1);
        for (int i = 0; i < childrenToInsert; i++) {
          editionPool->clone(nodeToInsert, true);
          nodeToInsert = nodeToInsert.nextTree();
        }
        continue;
      } else {
        assert(childrenToInsert == 1);
        nodeToInsert = nodeToInsert.childAtIndex(1);
      }
    } else if (filter == Placeholder::Filter::First) {
      nodeToInsert = nodeToInsert.childAtIndex(0);
    } else {
      assert(filter == Placeholder::Filter::None);
    }
    editionPool->clone(nodeToInsert, true);
  }
  return EditionReference(top);
}
