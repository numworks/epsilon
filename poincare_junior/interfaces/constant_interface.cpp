#include "constant_interface.h"
#include "../node.h"

namespace Poincare {

void ConstantInterface::logAttributes(const TypeBlock * treeBlock, std::ostream & stream) const {
  stream << " value=\"" << ConstantExpressionInterface::Value(treeBlock) << "\"";
}

float ConstantExpressionInterface::approximate(const TypeBlock * treeBlock) const {
  return Value(treeBlock);
}

float ConstantExpressionInterface::Value(const TypeBlock * treeBlock) {
  const Block * typeBlock = treeBlock->next();
  const ConstantInterface::Type type = static_cast<ConstantInterface::Type>(static_cast<const ValueBlock *>(typeBlock)->value());
  switch (type) {
  case ConstantInterface::Type::Pi:
    return 3.14;
  case ConstantInterface::Type::E:
    return 2.72;
  default:
    assert(false);
  }
}


}
