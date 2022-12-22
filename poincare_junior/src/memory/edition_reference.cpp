#include "edition_reference.h"
#include "edition_pool.h"
#include "node_constructor.h"
#include "node_iterator.h"
#include <string.h>

namespace Poincare {

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
    node().log(std::cout, true, 1, true);
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

Node EditionReference::node() const {
  Node n = EditionPool::sharedEditionPool()->nodeForIdentifier(m_identifier);
  assert(!n.isUninitialized());
  return n;
}

void EditionReference::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (std::pair<EditionReference, int> child : NodeIterator::Children<Forward, Editable>(*this)) {
    std::get<EditionReference>(child).recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

void EditionReference::replaceBy(Node newNode, bool oldIsTree, bool newIsTree) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  Node oldNode = node();
  int oldSize = oldIsTree ? oldNode.treeSize() : oldNode.nodeSize();
  int newSize = newIsTree ? newNode.treeSize() : newNode.nodeSize();
  Block * oldBlock = oldNode.block();
  Block * newBlock = newNode.block();
  if (pool->contains(newNode.block())) {
    if (oldIsTree && newNode.hasAncestor(oldNode, false)) {
      oldSize -= newSize;
    }
    pool->moveBlocks(oldBlock, newBlock, newSize);
    pool->removeBlocks(oldBlock + newSize, oldSize);
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

void EditionReference::remove(bool isTree) {
  EditionPool::sharedEditionPool()->removeBlocks(block(), isTree ? node().treeSize() : node().nodeSize());
}

void EditionReference::insert(Node nodeToInsert, bool before, bool isTree) {
  Node destination = before ? node() : nextNode().node();
  EditionPool * pool = EditionPool::sharedEditionPool();
  size_t sizeToInsert = isTree ? nodeToInsert.treeSize() : nodeToInsert.nodeSize();
  if (pool->contains(nodeToInsert.block())) {
    pool->moveBlocks(destination.block(), nodeToInsert.block(), sizeToInsert);
  } else {
    pool->insertBlocks(destination.block(), nodeToInsert.block(), sizeToInsert);
  }
}

}

template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Addition, int>(int);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Multiplication, int>(int);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Constant, char16_t>(char16_t);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Power>();
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Subtraction>();
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Division>();
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::IntegerShort, int>(int);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::IntegerPosBig, int>(int);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::IntegerNegBig, int>(int);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Float, float>(float);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::MinusOne>();
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Set>(int);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Half>();
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Zero>();
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::One>();
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::Two>();
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::RationalShort>(int8_t, uint8_t);
template Poincare::EditionReference Poincare::EditionReference::Push<Poincare::BlockType::IntegerShort>(int8_t);
