#ifndef POINCARE_DIVISION_INTERFACE_H
#define POINCARE_DIVISION_INTERFACE_H

#include "expression_interface.h"
#include "interface.h"

namespace Poincare {

class DivisionInterface final : public Interface {
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex) {
    assert(blockIndex == 0);
    *block = DivisionBlock;
    return true;
  }
  static TypeBlock * PushNode() { return Interface::PushNode<DivisionInterface>(); }
  constexpr int numberOfChildren(const TypeBlock * block) const override { return k_numberOfChildren; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Division"; }
#endif

  constexpr static size_t k_numberOfBlocksInNode = 1;
  constexpr static int k_numberOfChildren = 2;
};

class DivisionExpressionInterface final : public ExpressionInterface {
public:
  void basicReduction(TypeBlock * treeBlock) const override;
  float approximate(const TypeBlock * treeBlock) const override;
};

}

#endif
