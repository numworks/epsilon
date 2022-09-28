#ifndef POINCARE_ADDITION_INTERFACE_H
#define POINCARE_ADDITION_INTERFACE_H

#include "n_ary_interface.h"

namespace Poincare {

class AdditionInterface final : public NAryInterface {
public:
   static constexpr size_t CreateNodeAtAddress(Block * address, uint8_t numberOfChildren) { return NAryInterface::CreateNodeAtAddress(address, AdditionBlock, numberOfChildren); }
  static TypeBlock * PushNode(uint8_t numberOfChildren) { return Interface::PushNode<AdditionInterface, k_numberOfBlocksInNode>(numberOfChildren); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Addition"; }
#endif
};

class AdditionExpressionInterface final : public NAryExpressionInterface {
public:
  float approximate(const TypeBlock * treeBlock) const override;
};

}

#endif

