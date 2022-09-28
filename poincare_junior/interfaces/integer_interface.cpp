#include "constant_interface.h"
#include "multiplication_interface.h"
#include "power_interface.h"
#include "../node.h"

namespace Poincare {

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
  for (size_t i = 0; i < Node(block).nodeSize() - IntegerInterface::k_minimalNumberOfBlocksInNode; i++) {
    value += valueBlock->value();
    valueBlock = static_cast<const ValueBlock *>(valueBlock->next());
  }
  return value;
}



}
