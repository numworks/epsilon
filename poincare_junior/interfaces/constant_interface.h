#ifndef POINCARE_CONSTANT_INTERFACE_H
#define POINCARE_CONSTANT_INTERFACE_H

#include "internal_expression_interface.h"
#include "interface.h"

namespace Poincare {

class ConstantInterface final : public Interface {
public:
  enum class Type : uint8_t {
    Pi,
    E,
    Undefined
  };

  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, char16_t name) {
    if (blockIndex == 0 || blockIndex == k_numberOfBlocksInNode - 1) {
      *block = ConstantBlock;
    } else {
      assert(blockIndex == 1);
      Type type = name == 'e' ? Type::E : name == u'π' ? Type::Pi : Type::Undefined;
      assert(type != ConstantInterface::Type::Undefined);
      *block = ValueBlock(static_cast<uint8_t>(type));
    }
    return blockIndex == k_numberOfBlocksInNode - 1;
  };
  static TypeBlock * PushNode(char16_t type) { return Interface::PushNode<ConstantInterface>(type); }

  constexpr size_t nodeSize(const TypeBlock * block, bool head = true) const override { return 3; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Constant"; }
  void logAttributes(const TypeBlock * treeBlock, std::ostream & stream) const override;
#endif

  static constexpr size_t k_numberOfBlocksInNode = 3;
};

class ConstantExpressionInterface final : public InternalExpressionInterface {
public:
  float approximate(const TypeBlock * treeBlock) const override;
  static float Value(const TypeBlock * treeBlock);
};
}

#endif
