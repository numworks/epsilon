#ifndef POINCARE_SUBTRACTION_INTERFACE_H
#define POINCARE_SUBTRACTION_INTERFACE_H

#include "expression_interface.h"
#include "interface.h"

namespace Poincare {

class SubtractionInterface final : public Interface {
public:
  constexpr static size_t CreateNodeAtAddress(Block * address) {
    *(address) = SubtractionBlock;
    return k_numberOfBlocksInNode;
  }
  static TypeBlock * PushNode() { return Interface::PushNode<SubtractionInterface, k_numberOfBlocksInNode>(); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Subtraction"; }
#endif
  constexpr int numberOfChildren(const TypeBlock * block) const override { return k_numberOfChildren; }
  static constexpr size_t k_numberOfBlocksInNode = 3;
  constexpr static int k_numberOfChildren = 2;
};

class SubtractionExpressionInterface final : public ExpressionInterface {
public:
  void basicReduction(TypeBlock * treeBlock) const override;
  float approximate(const TypeBlock * treeBlock) const override;
};

}

#endif
