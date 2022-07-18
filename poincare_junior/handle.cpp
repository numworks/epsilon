#include "handle.h"
#include "tree_sandbox.h"
#include "tree_block.h"

namespace Poincare {

HandleBuffer::HandleBuffer() {
  // TODO: explain the need of dynamic allocation
#if GHOST_REQUIRED
  new (&m_ghost) Ghost();
#else
  new (&m_integer) Integer();
#endif
}

HandleBuffer::~HandleBuffer() {
  (&m_handle)->~Handle();
}

template <typename T>
T Handle::Create(const TypeTreeBlock * treeBlock) {
  return T(treeBlock);
}

Handle * Handle::CreateHandle(const TypeTreeBlock * treeBlock) {
  static HandleBuffer s_handleBuffer;
  (&s_handleBuffer.m_handle)->~Handle();
  switch (treeBlock->type()) {
#if GHOST_REQUIRED
    case BlockType::Ghost:
      new (&s_handleBuffer.m_ghost) Ghost(treeBlock);
      return &s_handleBuffer.m_ghost;
#endif
    case BlockType::AdditionHead:
    case BlockType::AdditionTail:
      new (&s_handleBuffer.m_addition) Addition(treeBlock);
      return &s_handleBuffer.m_addition;
    case BlockType::MultiplicationHead:
    case BlockType::MultiplicationTail:
      new (&s_handleBuffer.m_multiplication) Multiplication(treeBlock);
      return &s_handleBuffer.m_multiplication;
    case BlockType::IntegerHead:
    case BlockType::IntegerTail:
      new (&s_handleBuffer.m_integer) Integer(treeBlock);
      return &s_handleBuffer.m_integer;
    case BlockType::Subtraction:
      new (&s_handleBuffer.m_subtraction) Subtraction(treeBlock);
      return &s_handleBuffer.m_subtraction;
    case BlockType::Division:
      new (&s_handleBuffer.m_division) Division(treeBlock);
      return &s_handleBuffer.m_division;
    case BlockType::Power:
      new (&s_handleBuffer.m_power) Power(treeBlock);
      return &s_handleBuffer.m_power;
    default:
      assert(false);
  }
  return nullptr;
}

/* Subtraction  */

Subtraction Subtraction::PushNode(TreeSandbox * sandbox) {
  return Subtraction(static_cast<TypeTreeBlock *>(sandbox->pushBlock(SubtractionBlock())));
}

void Subtraction::basicReduction(TreeSandbox * sandbox) {
  Addition newAddition = Addition::PushNode(sandbox, 2);
  sandbox->moveTree(m_typeTreeBlock->nextNode(), static_cast<TypeTreeBlock *>(sandbox->lastBlock()));
  Multiplication::PushNode(sandbox, 2);
  Integer::PushNode(sandbox, -1);
  sandbox->moveTree(m_typeTreeBlock->nextNode(), static_cast<TypeTreeBlock *>(sandbox->lastBlock()));
  sandbox->removeBlocks(m_typeTreeBlock, nodeSize());
  sandbox->moveTree(m_typeTreeBlock, newAddition.typeTreeBlock());
}

/* Division */

Division Division::PushNode(TreeSandbox * sandbox) {
  return Division(static_cast<TypeTreeBlock *>(sandbox->pushBlock(DivisionBlock())));
}

void Division::basicReduction(TreeSandbox * sandbox) {
  Multiplication newMultiplication = Multiplication::PushNode(sandbox, 2);
  sandbox->moveTree(m_typeTreeBlock->nextNode(), static_cast<TypeTreeBlock *>(sandbox->lastBlock()));
  Power::PushNode(sandbox);
  sandbox->moveTree(m_typeTreeBlock->nextNode(), static_cast<TypeTreeBlock *>(sandbox->lastBlock()));
  Integer::PushNode(sandbox, 1); // TODO: implement negative number
  sandbox->removeBlocks(m_typeTreeBlock, nodeSize());
  sandbox->moveTree(m_typeTreeBlock, newMultiplication.typeTreeBlock());
}

/* Integer */

void Integer::logAttributes(std::ostream & stream) const {
  stream << " value=\"" << value() << "\"";
}

size_t Integer::nodeSize() const {
  return m_typeTreeBlock->type() == BlockType::IntegerHead ? nodeSize(&TreeBlock::nextBlock) : nodeSize(&TreeBlock::previousBlock);
}

size_t Integer::nodeSize(NextStep step) const {
  return k_minimalNumberOfNodes + static_cast<ValueTreeBlock *>((m_typeTreeBlock->*step)())->value();
}

int Integer::value() const {
  int value = 0;
  TreeBlock * digitBlock;
  if (m_typeTreeBlock->type() == BlockType::IntegerHead) {
     digitBlock = m_typeTreeBlock->nextNthBlock(2);
  } else {
    digitBlock = m_typeTreeBlock->previousNthBlock(nodeSize() + 1 + 2);
  }
  ValueTreeBlock * valueBlock = static_cast<ValueTreeBlock *>(digitBlock);
  for (size_t i = 0; i < nodeSize() - k_minimalNumberOfNodes; i++) {
    value += valueBlock->value();
    valueBlock =  static_cast<ValueTreeBlock *>(valueBlock->nextBlock());
  }
  return value;
}

Integer Integer::PushNode(TreeSandbox * sandbox, int value) {
  TreeBlock * addressOfIntegerBlock = sandbox->lastBlock();
  sandbox->pushBlock(IntegerHeadBlock());
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
  sandbox->pushBlock(IntegerTailBlock());
  // Replace temporary node size
  sandbox->replaceBlock(addressOfNodeSizeBlock, ValueTreeBlock(nodeSize));
  return Integer(static_cast<TypeTreeBlock *>(addressOfIntegerBlock));
}

/* NAry */

void NAry::logAttributes(std::ostream & stream) const {
  stream << " numberOfChildren=\"" << numberOfChildren() << "\"";
}

int NAry::privateNumberOfChildren(BlockType headType) const {
  TreeBlock * numberOfChildrenBlock = m_typeTreeBlock->type() == headType ? m_typeTreeBlock->nextBlock() : m_typeTreeBlock->previousBlock();
  return static_cast<ValueTreeBlock *>(numberOfChildrenBlock)->value();
}

TypeTreeBlock * NAry::PushNode(TreeSandbox * sandbox, int numberOfChildren, TypeTreeBlock headBlock, TypeTreeBlock tailBlock) {
  TreeBlock * addressOfNAryBlock = sandbox->lastBlock();
  sandbox->pushBlock(headBlock);
  sandbox->pushBlock(ValueTreeBlock(numberOfChildren));
  sandbox->pushBlock(tailBlock);
  return static_cast<TypeTreeBlock *>(addressOfNAryBlock);
}

/* Addition */

Addition Addition::PushNode(TreeSandbox * sandbox, int numberOfChildren) {
  return Addition(NAry::PushNode(sandbox, numberOfChildren, AdditionHeadBlock(), AdditionTailBlock()));
}

int Addition::numberOfChildren() const {
  return privateNumberOfChildren(BlockType::AdditionHead);
}

/* Multiplication */

Multiplication Multiplication::PushNode(TreeSandbox * sandbox, int numberOfChildren) {
  return Multiplication(NAry::PushNode(sandbox, numberOfChildren, MultiplicationHeadBlock(), MultiplicationTailBlock()));
}

int Multiplication::numberOfChildren() const {
  return privateNumberOfChildren(BlockType::MultiplicationHead);
}

Handle Multiplication::distributeOverAddition(TreeSandbox * sandbox) {
  for (IndexedTypeTreeBlock indexedSubTree : m_typeTreeBlock->directChildren()) {
    if (indexedSubTree.m_block->type() == BlockType::AdditionHead) {
      // Create new addition that will be filled in the following loop
      Addition newAddition = Addition::PushNode(sandbox, Handle::Create<Addition>(indexedSubTree.m_block).numberOfChildren());
      for (IndexedTypeTreeBlock indexedAdditionChild : indexedSubTree.m_block->directChildren()) {
        // Create a multiplication
        TypeTreeBlock * multiplicationCopy = sandbox->copyTreeFromAddress(m_typeTreeBlock);
        // Find the addition to be replaced
        TypeTreeBlock * additionCopy = multiplicationCopy->childAtIndex(indexedSubTree.m_index);
        // Duplicate addition child
        TypeTreeBlock * additionChildCopy = additionCopy->childAtIndex(indexedAdditionChild.m_index);
        // Replace addition per its child
        sandbox->replaceTree(additionCopy, additionChildCopy);
        assert(multiplicationCopy->type() == BlockType::MultiplicationHead);
        Handle::Create<Multiplication>(multiplicationCopy).distributeOverAddition(sandbox);
      }
      sandbox->replaceTree(m_typeTreeBlock, newAddition.typeTreeBlock());
      return newAddition;
    }
  }
  return *this;
}

/* Power */

Power Power::PushNode(TreeSandbox * sandbox) {
  return Power(static_cast<TypeTreeBlock *>((sandbox->pushBlock(PowerBlock()))));
}

}
