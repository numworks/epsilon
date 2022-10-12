#include "edition_reference.h"
#include "edition_pool.h"
#include <string.h>

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

void EditionReference::replaceBy(EditionReference t, bool trees) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  Node oldNode = node();
  Node newNode = t.node();
  int oldSize = trees ? oldNode.treeSize() : oldNode.nodeSize();
  int newSize = trees ? newNode.treeSize() : newNode.nodeSize();
  Block * oldBlock = oldNode.block();
  Block * newBlock = newNode.block();
  if (pool->contains(newNode.block())) {
    if (newNode.hasAncestor(oldNode, false)) {
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

void EditionReference::remove(bool removeTree) {
  EditionPool::sharedEditionPool()->removeBlocks(block(), removeTree ? node().treeSize() : node().nodeSize());
}

void EditionReference::insert(EditionReference t, bool before, bool insertTree) {
  Node destination = before ? t.node() : t.node().nextNode();
  EditionPool * pool = EditionPool::sharedEditionPool();
  size_t sizeToInsert = insertTree ? node().treeSize() : node().nodeSize();
  // TODO: should we handle !pool->contains(block())?
  if (pool->contains(block())) {
    pool->moveBlocks(destination.block(), block(), sizeToInsert);
  } else {
    pool->insertBlocks(destination.block(), block(), sizeToInsert);
  }
}

}
