#include "pattern_matching.h"

#include "../n_ary.h"

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
      if (result.getNode(tag).isUninitialized()) {
        if (!(Placeholder::MatchesNode(node, currentNode))) {
          return Context();
        }
        result.setNode(tag, currentNode);
      } else if (!result.getNode(tag).treeIsIdenticalTo(currentNode)) {
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
    Placeholder::CreateFilter filter = Placeholder::NodeToCreateFilter(node);
    Node nodeToInsert = context.getNode(tag);
    assert(!nodeToInsert.isUninitialized());
    if (filter == Placeholder::CreateFilter::NonFirstChild) {
      int childrenToInsert = nodeToInsert.numberOfChildren() - 1;
      if (childrenToInsert > 1) {
        assert(nodeToInsert.isNAry());
        NAry::SetNumberOfChildren(editionPool->clone(nodeToInsert, false),
                                  childrenToInsert);
        nodeToInsert = nodeToInsert.childAtIndex(1);
        /* TODO: this could be optimized. We compute the treeSize multiple times
         * (in clone and in nextTree) with no need - this is a big memcpy that
         * could be done without looping through children (even if we need to
         * add an API wrapping the memcpy in EditionPool to ensure to log the
         * copy there) */
        for (int i = 0; i < childrenToInsert; i++) {
          editionPool->clone(nodeToInsert, true);
          nodeToInsert = nodeToInsert.nextTree();
        }
        continue;
      } else {
        assert(childrenToInsert == 1);
        nodeToInsert = nodeToInsert.childAtIndex(1);
      }
    } else if (filter == Placeholder::CreateFilter::FirstChild) {
      nodeToInsert = nodeToInsert.childAtIndex(0);
    } else {
      assert(filter == Placeholder::CreateFilter::None);
    }
    editionPool->clone(nodeToInsert, true);
  }
  return EditionReference(top);
}
