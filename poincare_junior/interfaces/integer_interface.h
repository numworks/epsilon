#ifndef POINCARE_INTEGER_INTERFACE_H
#define POINCARE_INTEGER_INTERFACE_H

#include "interface.h"
#include "internal_expression_interface.h"

namespace Poincare {

class IntegerInterface final : public Interface {
friend class IntegerExpressionInterface;
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, unsigned long long value) {
    if (blockIndex == 0) {
      *block = IntegerBlock;
      return false;
    } else {
      size_t digitIndex = blockIndex - 1;
      uint8_t leftValue = value;
      for (size_t i = 0; i < digitIndex; i++) {
        // TODO: optimize?
        leftValue /= k_maxValue;
      }
      if (leftValue == 0) {
        *block = IntegerBlock;
        return true;
      }
      uint8_t digit = leftValue % k_maxValue;
      *block = ValueBlock(digit);
      return false;
    }
  }
  static TypeBlock * PushNode(int value) { return Interface::PushNode<IntegerInterface>(value); }

#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Integer"; }
  void logAttributes(const TypeBlock * treeBlock, std::ostream & stream) const override;
#endif

  constexpr static size_t k_minimalNumberOfBlocksInNode = 4;
private:
  constexpr static size_t k_maxValue = 1 << 8;
  constexpr size_t nodeSize(const TypeBlock * block, bool head = true) const override {
    return k_minimalNumberOfBlocksInNode + static_cast<uint8_t>(*(head ? block->next() : block->previous()));
  }
};

class IntegerExpressionInterface final : public InternalExpressionInterface {
public:
  float approximate(const TypeBlock * treeBlock) const override;
  static int Value(const TypeBlock * treeBlock);
};

}

#endif

