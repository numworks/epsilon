#include "edition_reference.h"
#include "edition_pool.h"
#include "node_iterator.h"
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

void EditionReference::insert(Node n, bool before, bool isTree) {
  Node destination = before ? n : n.nextNode();
  EditionPool * pool = EditionPool::sharedEditionPool();
  size_t sizeToInsert = isTree ? node().treeSize() : node().nodeSize();
  // TODO: should we handle !pool->contains(block())?
  if (pool->contains(block())) {
    pool->moveBlocks(destination.block(), block(), sizeToInsert);
  } else {
    pool->insertBlocks(destination.block(), block(), sizeToInsert);
  }
}

}
