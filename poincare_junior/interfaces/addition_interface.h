#ifndef POINCARE_ADDITION_INTERFACE_H
#define POINCARE_ADDITION_INTERFACE_H

#include "n_ary_interface.h"

namespace Poincare {

class AdditionInterface final : public NAryInterface {
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, uint8_t numberOfChildren) { return NAryInterface::CreateBlockAtIndex(block, blockIndex, numberOfChildren, AdditionBlock); }
  static TypeBlock * PushNode(uint8_t numberOfChildren) { return Interface::PushNode<AdditionInterface>(numberOfChildren); }
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

