#include "handle.h"
#include "tree_sandbox.h"
#include "tree_block.h"

namespace Poincare {

HandleBuffer::HandleBuffer() {
  new (&m_ghost) Ghost();
}

HandleBuffer::~HandleBuffer() {
  (&m_handle)->~Handle();
}

Handle * Handle::CreateFromBlock(const TypeTreeBlock * treeBlock) {
  static HandleBuffer s_handle;
  (&s_handle.m_handle)->~Handle();
  switch (treeBlock->type()) {
    case BlockType::Ghost:
      new (&s_handle.m_ghost) Ghost(treeBlock);
      return &s_handle.m_ghost;
    case BlockType::AdditionHead:
    case BlockType::AdditionTail:
      new (&s_handle.m_addition) Addition(treeBlock);
      return &s_handle.m_addition;
    case BlockType::MultiplicationHead:
    case BlockType::MultiplicationTail:
      new (&s_handle.m_multiplication) Multiplication(treeBlock);
      return &s_handle.m_multiplication;
    case BlockType::IntegerHead:
    case BlockType::IntegerTail:
      new (&s_handle.m_integer) Integer(treeBlock);
      return &s_handle.m_integer;
    default:
      assert(false);
  }
}

/* Integer */

void Integer::logAttributes(std::ostream & stream) const {
  stream << " value=\"" << value() << "\"";
}

size_t Integer::nodeSize() const {
  return m_typeTreeBlock->type() == BlockType::IntegerHead ? nodeSize(&TreeBlock::nextBlock) : nodeSize(&TreeBlock::previousBlock);
}

size_t Integer::nodeSize(NextStep step) const {
  size_t size = k_minimalNumberOfNodes;
  return 4 + static_cast<ValueTreeBlock *>((m_typeTreeBlock->*step)())->value();
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

}
