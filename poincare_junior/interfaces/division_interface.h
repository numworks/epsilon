#ifndef POINCARE_DIVISION_INTERFACE_H
#define POINCARE_DIVISION_INTERFACE_H

#include "expression_interface.h"
#include "interface.h"

namespace Poincare {

class DivisionInterface final : public Interface {
public:
   static constexpr size_t CreateNodeAtAddress(Block * address) {
    *(address) = DivisionBlock;
    return k_numberOfBlocksInNode;
  }
  static TypeBlock * PushNode() { return Interface::PushNode<DivisionInterface, k_numberOfBlocksInNode>(); }
  constexpr int numberOfChildren(const TypeBlock * block) const override { return k_numberOfChildren; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Division"; }
#endif

  constexpr static size_t k_numberOfBlocksInNode = 3;
  constexpr static int k_numberOfChildren = 2;
};

class DivisionExpressionInterface final : public ExpressionInterface {
public:
  void basicReduction(TypeBlock * treeBlock) const override;
  float approximate(const TypeBlock * treeBlock) const override;
};

}

#endif
