#include "edition_reference.h"

#include <ion/unicode/code_point.h>
#include <poincare_junior/include/poincare.h>
#include <poincare_junior/src/expression/k_creator.h>
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

Node EditionReference::replaceBy(Node newNode, bool oldIsTree, bool newIsTree) {
  EditionPool* pool = EditionPool::sharedEditionPool();
  Node oldNode = *this;
  int oldSize = oldIsTree ? oldNode.treeSize() : oldNode.nodeSize();
  int newSize = newIsTree ? newNode.treeSize() : newNode.nodeSize();
  Block* oldBlock = oldNode.block();
  Block* newBlock = newNode.block();
  if (oldBlock == newBlock && oldSize == newSize) {
    return newNode;
  }
  Block* finalBlock = oldBlock;
  if (pool->contains(newNode.block())) {
    // Fractal scheme
    assert(!(newIsTree && oldNode.hasAncestor(newNode, true)));
    if (oldIsTree && newNode.hasAncestor(oldNode, true)) {
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
    Log(LoggerType::Edition, "Replace", finalBlock, newSize);
#endif
  }
  return Node(finalBlock);
}

EditionReference EditionReference::matchAndCreate(const Node pattern,
                                                  const Node structure) const {
  PatternMatching::Context ctx = PatternMatching::Match(pattern, *this);
  if (ctx.isUninitialized()) {
    return EditionReference();
  }
  return PatternMatching::Create(structure, ctx);
}

EditionReference EditionReference::matchAndReplace(const Node pattern,
                                                   const Node structure) {
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
  PatternMatching::Context ctx = PatternMatching::Match(pattern, *this);
  if (ctx.isUninitialized()) {
    return *this;
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
    if (ctx.getNode(i).isUninitialized()) {
      continue;
    }
    initializedPlaceHolders += 1;
    treeNext.insertTreeBeforeNode(0_e);
    // Keep track of placeholder matches before detaching them
    placeholders[i] = EditionReference(ctx.getNode(i));
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
    // Warning : From this point forward, ctx.getNode(i) is no longer reliable.
    ctx.setNode(i, Node());
    placeholders[i].detachTree();
  }

  // EditionPool: ..... | *{2} +{2} 0 0 0 | .... _{3} x y z

  // Step 3 - Replace with placeholder matches only
  replaceTreeByTree(placeholderMatches);
  *this = placeholderMatches;

  // EditionPool: ..... | _{3} x y z | ....

  // Step 4 - Update context with new placeholder matches position
  for (uint8_t i = 0; i < Placeholder::Tag::NumberOfTags; i++) {
    ctx.setNode(i, static_cast<Node>(placeholders[i]));
  }

  // Step 5 - Build the PatternMatching replacement
  EditionReference createdRef = PatternMatching::Create(structure, ctx);

  // EditionPool: ..... | _{3} x y z | .... +{2} *{2} x z *{2} y z

  // Step 6 - Replace with created structure
  replaceTreeByTree(createdRef);
  *this = createdRef;

  // EditionPool: ..... | +{2} *{2} x z *{2} y z | ....
  return *this;
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
  Log(LoggerType::Edition, "Detach", destination - sizeToMove, sizeToMove,
      source);
#endif
}

}  // namespace PoincareJ
