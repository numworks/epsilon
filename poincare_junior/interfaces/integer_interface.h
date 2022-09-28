#ifndef POINCARE_INTEGER_INTERFACE_H
#define POINCARE_INTEGER_INTERFACE_H

#include "interface.h"
#include "internal_expression_interface.h"

namespace Poincare {

class IntegerInterface final : public Interface {
friend class IntegerExpressionInterface;
public:
  static constexpr size_t CreateNodeAtAddress(Block * address, unsigned long long value) {
    *(address) = IntegerBlock;
    int nodeSize = 0;
    while (value != 0) {
      uint8_t digit = value % k_maxValue;
      *(address + nodeSize + 2) = ValueBlock(digit);
      value = value / k_maxValue;
      nodeSize++;
    }
    *(address + 1) = nodeSize;
    *(address + nodeSize + 2) = nodeSize;
    *(address + nodeSize + 3) = IntegerBlock;
    return nodeSize + k_minimalNumberOfBlocksInNode;
  }
  static TypeBlock * PushNode(int value) { return Interface::PushNode<IntegerInterface, k_minimalNumberOfBlocksInNode + sizeof(unsigned long long)/sizeof(Block)>(value); }

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

