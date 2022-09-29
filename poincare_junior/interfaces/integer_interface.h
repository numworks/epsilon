#ifndef POINCARE_INTEGER_INTERFACE_H
#define POINCARE_INTEGER_INTERFACE_H

#include "interface.h"
#include "internal_expression_interface.h"

namespace Poincare {

class IntegerInterface final : public Interface {
friend class IntegerExpressionInterface;
public:
  static TypeBlock * PushNode(int value);

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Integer"; }
  void logAttributes(const TypeBlock * treeBlock, std::ostream & stream) const override;
#endif

  constexpr static size_t k_numberOfMetaBlocksInNode = 4;
private:
  constexpr static size_t k_maxValue = 1 << 8;
  constexpr size_t nodeSize(const TypeBlock * block, bool head = true) const override {
    uint8_t numberOfDigits = static_cast<uint8_t>(*(head ? block->next() : block->previous()));
    return numberOfDigits == 0 ? k_numberOfMetaBlocksInNode - 1 : k_numberOfMetaBlocksInNode + numberOfDigits;
  }
};

class IntegerExpressionInterface final : public InternalExpressionInterface {
public:
  float approximate(const TypeBlock * treeBlock) const override;
  static int Value(const TypeBlock * treeBlock);
};

}

#endif

