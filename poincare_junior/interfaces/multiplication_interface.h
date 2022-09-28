#ifndef POINCARE_MULTIPLICATION_INTERFACE_H
#define POINCARE_MULTIPLICATION_INTERFACE_H

#include "n_ary_interface.h"

namespace Poincare {

class MultiplicationInterface final : public NAryInterface {
public:
   static constexpr size_t CreateNodeAtAddress(Block * address, uint8_t numberOfChildren) { return NAryInterface::CreateNodeAtAddress(address, MultiplicationBlock, numberOfChildren); }
  static TypeBlock * PushNode(uint8_t numberOfChildren) { return Interface::PushNode<MultiplicationInterface, k_numberOfBlocksInNode>(numberOfChildren); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Multiplication"; }
#endif
};

class MultiplicationExpressionInterface final : public NAryExpressionInterface {
public:
  float approximate(const TypeBlock * treeBlock) const override;
  static TypeBlock * DistributeOverAddition(TypeBlock * treeBlock);
};

}

#endif
