#include "constant_interface.h"
#include "multiplication_interface.h"
#include "power_interface.h"
#include "../edition_pool.h"

namespace Poincare {

TypeBlock * IntegerInterface::PushNode(int value) {
  EditionPool * pool = EditionPool::sharedEditionPool();
  TypeBlock * newNode = static_cast<TypeBlock *>(pool->lastBlock());
  pool->pushBlock(IntegerBlock);
  pool->pushBlock(ValueBlock(0)); // Dummy NumberOfDigits block
  uint8_t numberOfDigits = 0;
  while (value > 0) {
    pool->pushBlock(ValueBlock(value % k_maxValue));
    value /= k_maxValue;
    numberOfDigits++;
  }
  if (numberOfDigits > 0) {
    pool->pushBlock(ValueBlock(numberOfDigits));
    pool->replaceBlock(newNode + 1, ValueBlock(numberOfDigits));
  }
  pool->pushBlock(IntegerBlock);
  return newNode;
}

void IntegerInterface::logAttributes(const TypeBlock * block, std::ostream & stream) const {
  stream << " value=\"" << IntegerExpressionInterface::Value(block) << "\"";
}

float IntegerExpressionInterface::approximate(const TypeBlock * block) const {
  return Value(block);
}

int IntegerExpressionInterface::Value(const TypeBlock * block) {
  int value = 0;
  const Block * digitBlock = block->nextNth(2);
  const ValueBlock * valueBlock = static_cast<const ValueBlock *>(digitBlock);
  for (size_t i = 0; i < Node(block).nodeSize() - IntegerInterface::k_numberOfMetaBlocksInNode; i++) {
    value += valueBlock->value();
    valueBlock = static_cast<const ValueBlock *>(valueBlock->next());
  }
  return value;
}



}
