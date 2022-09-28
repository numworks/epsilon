#ifndef POINCARE_N_ARY_INTERFACE_H
#define POINCARE_N_ARY_INTERFACE_H

#include "interface.h"
#include "internal_expression_interface.h"

namespace Poincare {

class NAryInterface : public Interface {
public:
#if POINCARE_TREE_LOG
  void logAttributes(const TypeBlock * treeBlock, std::ostream & stream) const override;
#endif
  constexpr int numberOfChildren(const TypeBlock * block) const override {
    return static_cast<uint8_t>(*block->next());
  }
  constexpr size_t nodeSize(const TypeBlock * block, bool head = true) const override { return 3; }
  static constexpr size_t k_numberOfBlocksInNode = 3;
protected:
   static constexpr size_t CreateNodeAtAddress(Block * address, TypeBlock typeBlock, uint8_t numberOfChildren) {
    *(address++) = typeBlock;
    *(address++) = ValueBlock(numberOfChildren);
    *(address) = typeBlock;
    return k_numberOfBlocksInNode;
  }
};

class NAryExpressionInterface : public InternalExpressionInterface {
public:
  static TypeBlock * Flatten(TypeBlock * treeBlock);
};

}

#endif
