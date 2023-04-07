#include "edition_reference.h"

#include <ion/unicode/code_point.h>
#include <poincare_junior/include/poincare.h>
#include <string.h>

#include "edition_pool.h"
#include "node_iterator.h"
#include "pattern_matching.h"

namespace PoincareJ {

EditionReference::EditionReference(Node node) {
  if (node.isUninitialized()) {
    m_identifier = EditionPool::ReferenceTable::NoNodeIdentifier;
    return;
  }
  EditionPool* pool = EditionPool::sharedEditionPool();
  // TODO: maybe make an assertion(pool->contains(node.block()))
  // and force developers to write EditionReference(EditionPool::clone(2_e))
  if (!pool->contains(node.block()) && node.block() != pool->lastBlock()) {
    *this = EditionReference(pool->clone(node));
    return;
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

EditionReference::operator const Node() const {
  Node n = EditionPool::sharedEditionPool()->nodeForIdentifier(m_identifier);
  return n;
}

void EditionReference::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (auto [child, index] : NodeIterator::Children<Forward, Editable>(*this)) {
    child.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

void EditionReference::replaceBy(Node newNode, bool oldIsTree, bool newIsTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Node oldNode = *this;
  int oldSize = oldIsTree ? oldNode.treeSize() : oldNode.nodeSize();
  int newSize = newIsTree ? newNode.treeSize() : newNode.nodeSize();
  Block* oldBlock = oldNode.block();
  Block* newBlock = newNode.block();
  if (oldBlock == newBlock && oldIsTree == newIsTree) {
    return;
  }
  if (pool->contains(newNode.block())) {
    assert(
        !(newIsTree && oldNode.hasAncestor(newNode, true)));  // Fractal scheme
    if (oldIsTree && newNode.hasAncestor(oldNode, true)) {
      oldSize -= newSize;
    }
    pool->moveBlocks(oldBlock, newBlock, newSize);
    pool->removeBlocks(oldBlock > newBlock ? oldBlock : oldBlock + newSize,
                       oldSize);
#if POINCARE_POOL_VISUALIZATION
    Log(LoggerType::Edition, "Replace", oldBlock, newSize, newBlock);
#endif
  } else {
    size_t minSize = std::min(oldSize, newSize);
    pool->replaceBlocks(oldBlock, newBlock, minSize);
    if (oldSize > newSize) {
      pool->removeBlocks(oldBlock + minSize, oldSize - newSize);
    } else {
      pool->insertBlocks(oldBlock + minSize, newBlock + minSize,
                         newSize - oldSize);
    }
#if POINCARE_POOL_VISUALIZATION
    Log(LoggerType::Edition, "Replace", oldBlock, newSize);
#endif
  }
}

EditionReference EditionReference::matchAndRewrite(const Node pattern,
                                                   const Node structure) {
  PatternMatching::Context ctx =
      PatternMatching::Match(pattern, static_cast<Node>(*this));
  if (ctx.isUninitialized()) {
    return *this;
  }
  return PatternMatching::Create(structure, ctx);
}

void EditionReference::remove(bool isTree) {
  Block* b = block();
  size_t size = isTree ? static_cast<Node>(*this).treeSize()
                       : static_cast<Node>(*this).nodeSize();
  EditionPool::sharedEditionPool()->removeBlocks(b, size);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Remove", nullptr, INT_MAX, b);
#endif
}

void EditionReference::insert(Node nodeToInsert, bool before, bool isTree) {
  Node destination = before ? static_cast<Node>(*this) : nextNode();
  EditionPool* pool = EditionPool::sharedEditionPool();
  size_t sizeToInsert =
      isTree ? nodeToInsert.treeSize() : nodeToInsert.nodeSize();
  if (pool->contains(nodeToInsert.block())) {
    pool->moveBlocks(destination.block(), nodeToInsert.block(), sizeToInsert);
#if POINCARE_POOL_VISUALIZATION
    Block* dst = destination.block();
    Block* addedBlock = dst >= nodeToInsert.block() ? dst - sizeToInsert : dst;
    Log(LoggerType::Edition, "Insert", addedBlock, sizeToInsert,
        nodeToInsert.block());
#endif
  } else {
    pool->insertBlocks(destination.block(), nodeToInsert.block(), sizeToInsert);
#if POINCARE_POOL_VISUALIZATION
    Log(LoggerType::Edition, "Insert", destination.block(), sizeToInsert);
#endif
  }
}

void EditionReference::detach(bool isTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Block* destination = pool->lastBlock();
  size_t sizeToMove = isTree ? static_cast<Node>(*this).treeSize()
                             : static_cast<Node>(*this).nodeSize();
  Block* source = static_cast<Node>(*this).block();
  pool->moveBlocks(destination, source, sizeToMove);
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Detach", destination - sizeToMove, sizeToMove, source);
#endif
}

}  // namespace PoincareJ
