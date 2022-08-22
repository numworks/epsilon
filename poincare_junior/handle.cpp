#include "handle.h"
#include "tree_sandbox.h"
#include "tree_block.h"

namespace Poincare {

void ProjectionReduction(TypeTreeBlock * treeBlock, TypeTreeBlock * (*PushNode)(), TypeTreeBlock * (*PushInverseNode)()) {
  TreeSandbox * sandbox = TreeSandbox::sharedSandbox();
  // Rule a / b --> a * b^-1 (or a - b --> a + b * -1)
  // Create empty * (or +)
  TypeTreeBlock * newOperation = PushNode();
  // Move first child: a * (or a +-
  size_t childSize = treeBlock->nextNode()->treeSize();
  sandbox->moveTree(static_cast<TypeTreeBlock *>(sandbox->lastBlock()), treeBlock->nextNode(), &childSize);
  // The new pointer is offseted to reflect the previous block movement
  newOperation -= childSize;
  // Create empty * (or *)
  PushInverseNode();
  // Move second child: a * b^ (or a + b *)
  childSize = treeBlock->nextNode()->treeSize();
  sandbox->moveTree(static_cast<TypeTreeBlock *>(sandbox->lastBlock()), treeBlock->nextNode(), &childSize);
  // The new pointer is offseted to reflect the previous block movement
  newOperation -= childSize;
  // Complete: a * b^-1 (or a + b * -1)
  Integer::PushNode(111); // TODO: implement negative number
  // Remove / node (or -)
  size_t previousNodeSize = treeBlock->nodeSize();
  sandbox->removeBlocks(treeBlock, previousNodeSize);
  // The new pointer is offseted to reflect the previous block removal
  newOperation -= previousNodeSize;
  sandbox->moveTree(treeBlock, newOperation);
}

/* Subtraction  */

TypeTreeBlock * Subtraction::PushNode() {
  return static_cast<TypeTreeBlock *>(TreeSandbox::sharedSandbox()->pushBlock(SubtractionBlock()));
}

void Subtraction::basicReduction(TypeTreeBlock * treeBlock) const {
  assert(treeBlock->type() == BlockType::Subtraction);
  return ProjectionReduction(treeBlock,
      []() { return Addition::PushNode(2); },
      []() { return Multiplication::PushNode(2); }
    );
}

/* Division */

TypeTreeBlock * Division::PushNode() {
  return static_cast<TypeTreeBlock *>(TreeSandbox::sharedSandbox()->pushBlock(DivisionBlock()));
}

// TODO factorize with Subtraction
void Division::basicReduction(TypeTreeBlock * treeBlock) const {
  assert(treeBlock->type() == BlockType::Division);
  return ProjectionReduction(treeBlock,
      []() { return Multiplication::PushNode(2); },
      Power::PushNode
    );
}

/* Integer */

void Integer::logAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) const {
  stream << " value=\"" << Value(treeBlock) << "\"";
}

size_t Integer::nodeSize(const TypeTreeBlock * treeBlock, bool head) const {
  return head ? nodeSize(treeBlock, &TreeBlock::nextBlock) : nodeSize(treeBlock, &TreeBlock::previousBlock);
}

size_t Integer::nodeSize(const TypeTreeBlock * treeBlock, NextStep step) const {
  TypeTreeBlock * block = const_cast<TypeTreeBlock *>(treeBlock);
  return k_minimalNumberOfNodes + static_cast<ValueTreeBlock *>((block->*step)())->value();
}

int Integer::Value(const TypeTreeBlock * treeBlock) {
  int value = 0;
  const TreeBlock * digitBlock = treeBlock->nextNthBlock(2);
  const ValueTreeBlock * valueBlock = static_cast<const ValueTreeBlock *>(digitBlock);
  for (size_t i = 0; i < treeBlock->nodeSize() - k_minimalNumberOfNodes; i++) {
    value += valueBlock->value();
    valueBlock = static_cast<const ValueTreeBlock *>(valueBlock->nextBlock());
  }
  return value;
}

TypeTreeBlock * Integer::PushNode(int value) {
  TreeSandbox * sandbox = TreeSandbox::sharedSandbox();
  TypeTreeBlock * integerFirstBlock = static_cast<TypeTreeBlock *>(sandbox->pushBlock(IntegerBlock()));
  // Temporary node size
  size_t nodeSize = 0;
  TreeBlock * addressOfNodeSizeBlock = sandbox->lastBlock();
  sandbox->pushBlock(ValueTreeBlock(0));
  while (value != 0) {
    uint8_t digit = value % k_maxValue;
    sandbox->pushBlock(ValueTreeBlock(digit));
    value = value / k_maxValue;
    nodeSize++;
  }
  sandbox->pushBlock(ValueTreeBlock(nodeSize));
  sandbox->pushBlock(IntegerBlock());
  // Replace temporary node size
  sandbox->replaceBlock(addressOfNodeSizeBlock, ValueTreeBlock(nodeSize));
  return integerFirstBlock;
}

/* NAry */

void NAry::logAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) const {
  stream << " numberOfChildren=\"" << treeBlock->numberOfChildren() << "\"";
}

int NAry::numberOfChildren(const TypeTreeBlock * treeBlock) const {
  return static_cast<const ValueTreeBlock *>(treeBlock->nextBlock())->value();
}

TypeTreeBlock * NAry::PushNode(int numberOfChildren, TypeTreeBlock blockType) {
  TreeSandbox * sandbox = TreeSandbox::sharedSandbox();
  TypeTreeBlock * addressOfNAryBlock = static_cast<TypeTreeBlock *>(sandbox->pushBlock(blockType));
  sandbox->pushBlock(ValueTreeBlock(numberOfChildren));
  sandbox->pushBlock(blockType);
  return addressOfNAryBlock;
}

/* Addition */

TypeTreeBlock * Addition::PushNode(int numberOfChildren) {
  return NAry::PushNode(numberOfChildren, AdditionBlock());
}

int Addition::CollectChildren(TypeTreeBlock * treeBlock) {
  TreeSandbox * sandbox = TreeSandbox::sharedSandbox();
  int nbChildren = 0;
  for (IndexedTypeTreeBlock indexedSubTree : treeBlock->directChildren()) {
    if (indexedSubTree.m_block->type() == BlockType::Addition) {
      nbChildren += CollectChildren(indexedSubTree.m_block);
    } else {
      nbChildren++;
      sandbox->copyTreeFromAddress(indexedSubTree.m_block);
    }
  }
  return nbChildren;
}

TypeTreeBlock * Addition::Merge(TypeTreeBlock * treeBlock) {
  TreeSandbox * sandbox = TreeSandbox::sharedSandbox();
  TypeTreeBlock * newAddition = Addition::PushNode(0);
  int nbChildren = CollectChildren(treeBlock);
  // update children count
  sandbox->replaceBlock(newAddition + 1, ValueTreeBlock(nbChildren));
  return newAddition;
}

/* Multiplication */

TypeTreeBlock * Multiplication::PushNode(int numberOfChildren) {
  return NAry::PushNode(numberOfChildren, MultiplicationBlock());
}

TypeTreeBlock * Multiplication::DistributeOverAddition(TypeTreeBlock * treeBlock) {
  TreeSandbox * sandbox = TreeSandbox::sharedSandbox();
  for (IndexedTypeTreeBlock indexedSubTree : treeBlock->directChildren()) {
    if (indexedSubTree.m_block->type() == BlockType::Addition) {
      // Create new addition that will be filled in the following loop
      TypeTreeBlock * newAddition = Addition::PushNode(indexedSubTree.m_block->numberOfChildren());
      for (IndexedTypeTreeBlock indexedAdditionChild : indexedSubTree.m_block->directChildren()) {
        // Create a multiplication
        TypeTreeBlock * multiplicationCopy = sandbox->copyTreeFromAddress(treeBlock);
        // Find the addition to be replaced
        TypeTreeBlock * additionCopy = multiplicationCopy->childAtIndex(indexedSubTree.m_index);
        // Find addition child to replace with
        TypeTreeBlock * additionChildCopy = additionCopy->childAtIndex(indexedAdditionChild.m_index);
        // Replace addition per its child
        sandbox->replaceTree(additionCopy, additionChildCopy);
        assert(multiplicationCopy->type() == BlockType::Multiplication);
        Multiplication::DistributeOverAddition(multiplicationCopy);
      }
      sandbox->replaceTree(treeBlock, newAddition);
      return treeBlock;
    }
  }
  return treeBlock;
}

/* Power */

TypeTreeBlock * Power::PushNode() {
  return static_cast<TypeTreeBlock *>(TreeSandbox::sharedSandbox()->pushBlock(PowerBlock()));
}

}
