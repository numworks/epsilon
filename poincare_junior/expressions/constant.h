#ifndef POINCARE_EXPRESSIONS_CONSTANT_H
#define POINCARE_EXPRESSIONS_CONSTANT_H

#include "expression.h"
#include "../value_block.h"

namespace Poincare {

class Constant final : public Expression {
public:
  enum class Type : uint8_t {
    Pi,
    E,
    Undefined
  };

  static constexpr size_t k_numberOfBlocksInNode = 3;
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, char16_t name) {
    if (blockIndex == 0 || blockIndex == k_numberOfBlocksInNode - 1) {
      *block = ConstantBlock;
    } else {
      assert(blockIndex == 1);
      Type type = name == 'e' ? Type::E : name == u'π' ? Type::Pi : Type::Undefined;
      assert(type != Constant::Type::Undefined);
      *block = ValueBlock(static_cast<uint8_t>(type));
    }
    return blockIndex == k_numberOfBlocksInNode - 1;
  };

  static float Value(const TypeBlock * typeBlock);
};

}

#endif
