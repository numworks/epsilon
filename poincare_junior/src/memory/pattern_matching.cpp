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
  std::cout << "Context: \n  A:";
  (*this)[PlaceholderTag::A].log(std::cout, true, 2);
  std::cout << "\n  B:";
  (*this)[PlaceholderTag::B].log(std::cout, true, 2);
  std::cout << "\n  C:";
  (*this)[PlaceholderTag::C].log(std::cout, true, 2);
  std::cout << "\n";
}
#endif

PatternMatching::Context PatternMatching::Match(const Node pattern, Node source,
                                                Context result) {
  Pool::Nodes patternNodes = Pool::Nodes(
      pattern.block(), pattern.nextTree().block() - pattern.block());
  for (const Node node : patternNodes) {
    if (node.type() == BlockType::Placeholder) {
      PlaceholderTag placeholder = static_cast<PlaceholderTag>(
          static_cast<uint8_t>(*node.block()->next()));
      if (result[placeholder].isUninitialized()) {
        result[placeholder] = source;
      } else if (!result[placeholder].treeIsIdenticalTo(source)) {
        return Context();
      }
      source = source.nextTree();
    } else {
      if (!node.isIdenticalTo(source)) {
        return Context();
      }
      source = source.nextNode();
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
    if (node.type() == BlockType::Placeholder) {
      editionPool->clone(context[static_cast<PlaceholderTag>(
                             static_cast<uint8_t>(*node.block()->next()))],
                         true);
    } else {
      editionPool->clone(node, false);
    }
  }
  return EditionReference(top);
}
