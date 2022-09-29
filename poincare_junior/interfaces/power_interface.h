#ifndef POINCARE_POWER_INTERFACE_H
#define POINCARE_POWER_INTERFACE_H

#include "interface.h"
#include "internal_expression_interface.h"

namespace Poincare {

class PowerInterface final : public Interface {
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex) {
    assert(blockIndex == 0);
    *block = PowerBlock;
    return true;
  }
  static TypeBlock * PushNode() { return Interface::PushNode<PowerInterface>(); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Power"; }
#endif
  constexpr int numberOfChildren(const TypeBlock * block) const override { return 2; }
  static constexpr size_t k_numberOfBlocksInNode = 1;
};

class PowerExpressionInterface final : public InternalExpressionInterface {
public:
  float approximate(const TypeBlock * treeBlock) const override;
};

}

#endif
