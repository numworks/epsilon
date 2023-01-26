#include "edition_reference.h"
#include "edition_pool.h"
#include "node_constructor.h"
#include "node_iterator.h"
#include "pattern_matching.h"
#include <ion/unicode/code_point.h>
#include <string.h>

namespace PoincareJ {

EditionReference::EditionReference(Node node) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  if (!pool->contains(node.block()) && node.block() != pool->lastBlock()) {
    node = pool->initFromTree(node);
  }
  m_identifier = EditionPool::sharedEditionPool()->referenceNode(node);
}

#if POINCARE_MEMORY_TREE_LOG
void EditionReference::log() const {
    std::cout << "id: " << m_identifier;
    static_cast<Node>(*this).log(std::cout, true, 1, true);
    std::cout << std::endl;
  }
#endif

template <BlockType blockType, typename... Types>
EditionReference EditionReference::Push(Types... args) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock * newNode = static_cast<TypeBlock *>(pool->lastBlock());

  size_t i = 0;
  bool endOfNode = false;
  do {
    Block block;
    endOfNode = NodeConstructor::CreateBlockAtIndexForType<blockType>(&block, i++, args...);
    pool->pushBlock(block);
  } while (!endOfNode);
  return EditionReference(Node(newNode));
}

EditionReference EditionReference::Clone(const Node node) {
  return EditionReference(EditionPool::sharedEditionPool()->initFromTree(node));
}

EditionReference::operator const Node() const {
  Node n = EditionPool::sharedEditionPool()->nodeForIdentifier(m_identifier);
  assert(!n.isUninitialized());
  return n;
}

EditionReference EditionReference::replaceNodeByNode(EditionReference t) {
  replaceNodeByNode(static_cast<Node>(t));
  return t;
}

EditionReference EditionReference::replaceNodeByTree(EditionReference t) {
  replaceNodeByTree(static_cast<Node>(t));
  return t;
}

EditionReference EditionReference::replaceTreeByNode(EditionReference t) {
  replaceTreeByNode(static_cast<Node>(t));
  return t;
}

EditionReference EditionReference::replaceTreeByTree(EditionReference t) {
  replaceTreeByTree(static_cast<Node>(t));
  return t;
}

void EditionReference::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (auto [child, index] : NodeIterator::Children<Forward, Editable>(*this)) {
    child.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

void EditionReference::replaceBy(Node newNode, bool oldIsTree, bool newIsTree) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  Node oldNode = *this;
  int oldSize = oldIsTree ? oldNode.treeSize() : oldNode.nodeSize();
  int newSize = newIsTree ? newNode.treeSize() : newNode.nodeSize();
  Block * oldBlock = oldNode.block();
  Block * newBlock = newNode.block();
  if (oldBlock == newBlock && oldIsTree == newIsTree) {
    return;
  }
  if (pool->contains(newNode.block())) {
    assert(!(newIsTree && oldNode.hasAncestor(newNode, true))); // Fractal scheme
    if (oldIsTree && newNode.hasAncestor(oldNode, true)) {
      oldSize -= newSize;
    }
    pool->moveBlocks(oldBlock, newBlock, newSize);
    pool->removeBlocks(oldBlock > newBlock ? oldBlock : oldBlock + newSize, oldSize);
  } else {
    size_t minSize = std::min(oldSize, newSize);
    memcpy(oldBlock, newBlock, minSize);
    if (oldSize > newSize) {
      pool->removeBlocks(oldBlock + minSize, oldSize - newSize);
    } else {
      pool->insertBlocks(oldBlock + minSize, newBlock + minSize, newSize - oldSize);
    }
  }
}

EditionReference EditionReference::matchAndRewrite(const Node pattern, const Node structure) {
  PatternMatching::Context ctx = PatternMatching::Match(pattern, static_cast<Node>(*this));
  if (ctx.isUninitialized()) {
    return *this;
  }
  return PatternMatching::Create(structure, ctx);
}

void EditionReference::remove(bool isTree) {
  EditionPool::sharedEditionPool()->removeBlocks(block(), isTree ? static_cast<Node>(*this).treeSize() : static_cast<Node>(*this).nodeSize());
}

void EditionReference::insert(Node nodeToInsert, bool before, bool isTree) {
  Node destination = before ? *this : nextNode();
  EditionPool * pool = EditionPool::sharedEditionPool();
  size_t sizeToInsert = isTree ? nodeToInsert.treeSize() : nodeToInsert.nodeSize();
  if (pool->contains(nodeToInsert.block())) {
    pool->moveBlocks(destination.block(), nodeToInsert.block(), sizeToInsert);
  } else {
    pool->insertBlocks(destination.block(), nodeToInsert.block(), sizeToInsert);
  }
}

void EditionReference::detach(bool isTree) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  Block * destination = pool->lastBlock();
  size_t sizeToMove = isTree ? static_cast<Node>(*this).treeSize() : static_cast<Node>(*this).nodeSize();
  pool->moveBlocks(destination, static_cast<Node>(*this).block(), sizeToMove);
}

}

template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Addition, int>(int);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Multiplication, int>(int);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Constant, char16_t>(char16_t);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Power>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Subtraction>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Division>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::IntegerShort>(int8_t);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::IntegerPosBig>(uint64_t);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::IntegerNegBig>(uint64_t);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Float, float>(float);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::MinusOne>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Set>(int);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Half>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Zero>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::One>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Two>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::RationalShort>(int8_t, uint8_t);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::Polynomial, int, int>(int, int);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::RackLayout, int>(int);
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::FractionLayout>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::ParenthesisLayout>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::VerticalOffsetLayout>();
template PoincareJ::EditionReference PoincareJ::EditionReference::Push<PoincareJ::BlockType::CodePointLayout, CodePoint>(CodePoint);
