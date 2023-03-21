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
  Node top = EditionPool::sharedEditionPool()->lastBlock();
  // TODO introduce a DFS iterator in node_iterator and use it here
  Pool::Nodes nodes = Pool::Nodes(
      structure.block(), structure.nextTree().block() - structure.block());
  for (const Node node : nodes) {
    if (node.type() == BlockType::Placeholder) {
      EditionReference::Clone(context[static_cast<PlaceholderTag>(static_cast<uint8_t>(*node.block()->next()))], true);
    } else {
      EditionReference::Clone(node, false);
    }
  }
  return EditionReference(top);
}
