#include "edition_reference.h"
#include "edition_pool.h"

namespace Poincare {

EditionReference::EditionReference(Node node) {
  m_identifier = EditionPool::sharedEditionPool()->referenceNode(node);
}

Node EditionReference::node() const {
  Node n = EditionPool::sharedEditionPool()->nodeForIdentifier(m_identifier);
  assert(!n.isUninitialized());
  return n;
}

EditionReference EditionReference::clone() const {
  return EditionReference(EditionPool::sharedEditionPool()->initFromTree(node()));
}

void EditionReference::replaceBy(EditionReference t) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  int oldSize = node().nodeSize();
  int newSize = t.node().nodeSize();
  Block * oldBlock = node().block();
  Block * newBlock = t.node().block();
  for (int i = 0; i < std::min(oldSize, newSize); i++) {
    pool->replaceBlock(oldBlock++, *(newBlock++));
  }
  if (oldSize > newSize) {
    pool->removeBlocks(oldBlock, oldSize - newSize);
  } else {
    pool->insertBlocks(oldBlock, newBlock, newSize - oldSize);
  }
}

void EditionReference::remove() {
  EditionPool::sharedEditionPool()->removeBlocks(node().block(), node().nodeSize());
}

void EditionReference::insert(EditionReference t, bool before) {
  Node destination = before ? t.node() : t.node().nextNode();
  EditionPool * pool = EditionPool::sharedEditionPool();
  if (pool->contains(node().block())) {
    pool->moveBlocks(destination.block(), node().block(), node().nodeSize());
  } else {
    pool->insertBlocks(destination.block(), node().block(), node().nodeSize());
  }
}

}
