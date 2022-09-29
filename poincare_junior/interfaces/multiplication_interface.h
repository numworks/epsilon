#ifndef POINCARE_MULTIPLICATION_INTERFACE_H
#define POINCARE_MULTIPLICATION_INTERFACE_H

#include "n_ary_interface.h"

namespace Poincare {

class MultiplicationInterface final : public NAryInterface {
public:
   constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, uint8_t numberOfChildren) { return NAryInterface::CreateBlockAtIndex(block, blockIndex, numberOfChildren, MultiplicationBlock); }
  static TypeBlock * PushNode(uint8_t numberOfChildren) { return Interface::PushNode<MultiplicationInterface>(numberOfChildren); }
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
