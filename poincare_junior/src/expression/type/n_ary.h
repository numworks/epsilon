#ifndef POINCARE_EXPRESSIONS_N_ARY_H
#define POINCARE_EXPRESSIONS_N_ARY_H

#include <poincare_junior/src/expression/expression.h>
#include <poincare_junior/src/memory/value_block.h>

namespace Poincare {

class NAry : public EExpression {
public:
  static constexpr size_t k_numberOfBlocksInNode = 3;
protected:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex, uint8_t numberOfChildren, TypeBlock typeBlock) {
    if (blockIndex == 0 || blockIndex == k_numberOfBlocksInNode - 1) {
      *block = typeBlock;
    } else {
      *block = ValueBlock(numberOfChildren);
    }
    return blockIndex == k_numberOfBlocksInNode - 1;
  }
  static TypeBlock * Flatten(TypeBlock * treeBlock);
};

}

#endif
