#include "context.h"

using namespace PoincareJ;

EditionReference Context::build(Node structure)  {
  EditionReference top(EditionPool::sharedEditionPool()->lastBlock());
  // TODO introduce a DFS iterator in node_iterator and use it here
  Pool::Nodes nodes = Pool::Nodes(structure.block(), structure.nextTree().block() - structure.block());
  for (const Node node : nodes) {
    if (node.type() == BlockType::Placeholder) {
      EditionPool::sharedEditionPool()->initFromTree(m_array[static_cast<uint8_t>(*node.block()->next())]);
    } else {
      for (const Block block : node.blocks()) {
        EditionPool::sharedEditionPool()->pushBlock(block);
      }
    }
  }
  return top;
}

Context Context::Match(const Node pattern, Node sourceNode) {
  Context result;
  Pool::Nodes patternNodes = Pool::Nodes(pattern.block(), pattern.nextTree().block() - pattern.block());
  for (const Node node : patternNodes) {
    if (node.type() == BlockType::Placeholder) {
      Placeholder placeholder = static_cast<Placeholder>(static_cast<uint8_t>(*node.block()->next()));
      if (result[placeholder].isUninitialized()) {
        result[placeholder] = sourceNode;
        sourceNode = sourceNode.nextTree();
      } else if (!result[placeholder].isIdenticalTo(sourceNode)) {
        return Context();
      }
    } else {
      sourceNode = sourceNode.nextNode();
    }
  }
  return result;
}

