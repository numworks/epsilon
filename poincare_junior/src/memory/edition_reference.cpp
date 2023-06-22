#include "edition_reference.h"

#include <ion/unicode/code_point.h>
#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/k_creator.h>
#include <string.h>

#include "edition_pool.h"
#include "node_iterator.h"
#include "pattern_matching.h"

namespace PoincareJ {

EditionReference::EditionReference(const Node* node) {
  if (!node) {
    m_identifier = EditionPool::ReferenceTable::NoNodeIdentifier;
    return;
  }
  EditionPool* pool = EditionPool::sharedEditionPool();
  // TODO: maybe make an assertion(pool->contains(node->block()))
  // and force developers to write EditionReference(EditionPool::clone(2_e))
  if (!pool->contains(node->block()) && node->block() != pool->lastBlock()) {
    *this = EditionReference(pool->clone(node));
    return;
  }
  m_identifier =
      EditionPool::sharedEditionPool()->referenceNode(const_cast<Node*>(node));
}

EditionReference::EditionReference(Node* node) {
  assert(EditionPool::sharedEditionPool()->contains(node->block()) ||
         node->block() == EditionPool::sharedEditionPool()->lastBlock());
  m_identifier = EditionPool::sharedEditionPool()->referenceNode(node);
}

#if POINCARE_MEMORY_TREE_LOG
void EditionReference::log() const {
  std::cout << "id: " << m_identifier;
  node()->log(std::cout, true, 1, true);
  std::cout << std::endl;
}
#endif

Node* EditionReference::node() const {
  return EditionPool::sharedEditionPool()->nodeForIdentifier(m_identifier);
}

void EditionReference::recursivelyEdit(InPlaceTreeFunction treeFunction) {
  for (auto [child, index] : NodeIterator::Children<Forward, Editable>(*this)) {
    child.recursivelyEdit(treeFunction);
  }
  (*treeFunction)(*this);
}

Node* EditionReference::cloneOver(const Node* newNode, bool oldIsTree,
                                  bool newIsTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Node* oldNode = *this;
  int oldSize = oldIsTree ? oldNode->treeSize() : oldNode->nodeSize();
  int newSize = newIsTree ? newNode->treeSize() : newNode->nodeSize();
  Block* oldBlock = oldNode->block();
  const Block* newBlock = newNode->block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return Node::FromBlocks(oldBlock);
  }
  Block* finalBlock = oldBlock;
  size_t minSize = std::min(oldSize, newSize);
  pool->replaceBlocks(oldBlock, newBlock, minSize);
  if (oldSize > newSize) {
    pool->removeBlocks(oldBlock + minSize, oldSize - newSize);
  } else {
    pool->insertBlocks(oldBlock + minSize, newBlock + minSize,
                       newSize - oldSize);
  }
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Replace", finalBlock, newSize);
#endif
  return Node::FromBlocks(finalBlock);
}

Node* EditionReference::moveOver(Node* newNode, bool oldIsTree,
                                 bool newIsTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Node* oldNode = *this;
  int oldSize = oldIsTree ? oldNode->treeSize() : oldNode->nodeSize();
  int newSize = newIsTree ? newNode->treeSize() : newNode->nodeSize();
  Block* oldBlock = oldNode->block();
  Block* newBlock = newNode->block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return Node::FromBlocks(oldBlock);
  }
  Block* finalBlock = oldBlock;
  assert(pool->contains(newNode->block()));
  // Fractal scheme
  assert(!(newIsTree && oldNode->hasAncestor(newNode, true)));
  if (oldIsTree && newNode->hasAncestor(oldNode, true)) {
    oldSize -= newSize;
  }
  pool->moveBlocks(oldBlock, newBlock, newSize);
  if (oldBlock > newBlock) {
    finalBlock -= newSize;
  }
  pool->removeBlocks(finalBlock + newSize, oldSize);
#if POINCARE_POOL_VISUALIZATION
  if (oldBlock < newBlock) {
    newBlock -= oldSize;
  }
  Log(LoggerType::Edition, "Replace", finalBlock, newSize, newBlock);
#endif
  return Node::FromBlocks(finalBlock);
}

EditionReference EditionReference::matchAndCreate(const Node* pattern,
                                                  const Node* structure) const {
  PatternMatching::Context ctx;
  if (!PatternMatching::Match(pattern, *this, &ctx)) {
    return EditionReference();
  }
  return PatternMatching::Create(structure, ctx);
}

bool EditionReference::matchAndReplace(const Node* pattern,
                                       const Node* structure) {
  /* TODO: When possible this could be optimized by deleting all non-placeholder
   * pattern nodes and then inserting all the non-placeholder structure nodes.
   * For example : Pattern : +{4} A 1 B C A     Structure : *{4} 2 B A A
   *                                                EditionPool : +{4} x 1 y z x
   * 1 - Only keep structure's matched placeholders
   *                                                EditionPool : y x
   * 2 - Insert structure Nodes
   *                                                EditionPool : *{4} 2 y x A
   * 3 - Replace duplicated placeholders
   *                                                EditionPool : *{4} 2 y x x
   *
   * Some difficulties:
   *  - Detect if it is possible : BA->AB isn't but ABCBA->BCA is.
   *  - Handle PlaceHolder's CreateFilter such as FirstChild.
   *  - Implement a method allowing the insertion of uncompleted nodes :
   *      void insert(Block * startSrc, Block * endSrc, Block * dst)
   */

  // Step 1 - Match the pattern
  PatternMatching::Context ctx;
  if (!PatternMatching::Match(pattern, *this, &ctx)) {
    return false;
  }
  EditionPool* editionPool = EditionPool::sharedEditionPool();
  /* Following this example :
   * this (EditionReference): (x + y) * z
   * pattern: (A + B) * C
   * structure: A * C + B * C
   *
   * EditionPool: ..... | *{2} +{2} x y z | ....
   * With :
   * - | delimiting this reference
   * - *{2} a two children multiplication node
   * - +{2} a two children addition
   * - _{2} a two children systemList */

  // Step 2 - Detach placeholder matches
  /* Create ZeroBlock for each context node to be detached so that tree size is
   * preserved. */
  EditionReference treeNext = nextTree();
  int initializedPlaceHolders = 0;
  EditionReference placeholders[Placeholder::Tag::NumberOfTags];
  for (uint8_t i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
    if (!ctx.getNode(i)) {
      continue;
    }
    for (int j = 0; j < ctx.getNumberOfTrees(i); j++) {
      initializedPlaceHolders++;
      treeNext.cloneTreeBeforeNode(0_e);
    }
    // Keep track of placeholder matches before detaching them
    int numberOfTrees = ctx.getNumberOfTrees(i);
    if (!ctx.getNode(i)) {
      placeholders[i] = EditionReference();
    } else if (numberOfTrees == 0) {
      // Use the last block so that placeholders[i] stays initialized
      placeholders[i] = EditionReference(editionPool->lastBlock());
    } else {
      placeholders[i] = EditionReference(ctx.getNode(i));
    }
    // Invalidate context before anything is detached.
    ctx.setNode(i, nullptr, numberOfTrees);
  }

  // EditionPool: ..... | *{2} +{2} x y z | 0 0 0 ....

  // Detach placeholder matches at the end of the EditionPool in a system list
  EditionReference placeholderMatches(
      editionPool->push<BlockType::SystemList>(initializedPlaceHolders));

  // EditionPool: ..... | *{2} +{2} x y z | 0 0 0 .... _{3}

  for (uint8_t i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
    if (placeholders[i].isUninitialized()) {
      continue;
    }
    // Get a Node to the first placeholder tree, and detach as many as necessary
    Node* trees = Node::FromBlocks(placeholders[i].block());
    // If the placeHolder matches the entire Tree, restore it after detaching.
    bool restoreReference = trees->block() == block();
    for (int j = 0; j < ctx.getNumberOfTrees(i); j++) {
      EditionReference(trees).detachTree();
    }
    if (restoreReference) {
      *this = EditionReference(trees);
    }
  }

  // EditionPool: ..... | *{2} +{2} 0 0 0 | .... _{3} x y z

  // Step 3 - Replace with placeholder matches only
  moveTreeOverTree(placeholderMatches);
  *this = placeholderMatches;

  // EditionPool: ..... | _{3} x y z | ....

  // Step 4 - Update context with new placeholder matches position
  for (uint8_t i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
    ctx.setNode(i, placeholders[i].node(), ctx.getNumberOfTrees(i));
  }

  // Step 5 - Build the PatternMatching replacement
  EditionReference createdRef = PatternMatching::Create(structure, ctx);

  // EditionPool: ..... | _{3} x y z | .... +{2} *{2} x z *{2} y z

  // Step 6 - Replace with created structure
  moveTreeOverTree(createdRef);
  *this = createdRef;

  // EditionPool: ..... | +{2} *{2} x z *{2} y z | ....
  return true;
}

void EditionReference::moveAt(Node* nodeToMove, bool before, bool newIsTree) {
  Node* destination = before ? node() : nextNode();
  EditionPool* pool = EditionPool::sharedEditionPool();
  size_t size = newIsTree ? nodeToMove->treeSize() : nodeToMove->nodeSize();
  assert(pool->contains(nodeToMove->block()));
  pool->moveBlocks(destination->block(), nodeToMove->block(), size);
#if POINCARE_POOL_VISUALIZATION
  Block* dst = destination.block();
  Block* addedBlock = dst >= nodeToInsert.block() ? dst - sizeToInsert : dst;
  Log(LoggerType::Edition, "Insert", addedBlock, sizeToInsert,
      nodeToInsert.block());
#endif
}

void EditionReference::cloneAt(const Node* nodeToClone, bool before,
                               bool newIsTree) {
  Node* destination = before ? node() : nextNode();
  EditionPool::sharedEditionPool()->insertBlocks(
      destination->block(), nodeToClone->block(),
      newIsTree ? nodeToClone->treeSize() : nodeToClone->nodeSize());
#if POINCARE_POOL_VISUALIZATION
  Log(LoggerType::Edition, "Insert", destination.block(), sizeToInsert);
#endif
}

void CloneNodeBeforeNode(EditionReference* target, const Node* nodeToClone) {
  Node* previousTarget = *target;
  target->cloneNodeBeforeNode(nodeToClone);
  *target = previousTarget;
}

void CloneTreeBeforeNode(EditionReference* target, const Node* treeToClone) {
  Node* previousTarget = *target;
  target->cloneTreeBeforeNode(treeToClone);
  *target = previousTarget;
}

void MoveNodeBeforeNode(EditionReference* target, Node* nodeToMove) {
  Node* previousTarget = *target;
  if (nodeToMove->block() < previousTarget->block()) {
    previousTarget =
        Node::FromBlocks(previousTarget->block() - nodeToMove->nodeSize());
  }
  target->moveNodeBeforeNode(nodeToMove);
  *target = previousTarget;
}

void MoveTreeBeforeNode(EditionReference* target, Node* treeToMove) {
  Node* previousTarget = *target;
  if (treeToMove->block() < previousTarget->block()) {
    previousTarget =
        Node::FromBlocks(previousTarget->block() - treeToMove->treeSize());
  }
  target->moveTreeBeforeNode(treeToMove);
  *target = previousTarget;
}

void SwapTrees(EditionReference* u, EditionReference* v) {
  if (u->block() > v->block()) {
    return SwapTrees(v, u);
  }
  Node* previousU = *u;
  Node* previousV = *v;
  MoveTreeBeforeNode(v, previousU);
  *u = EditionReference(previousU);
  MoveTreeBeforeNode(u, previousV);
}

}  // namespace PoincareJ
