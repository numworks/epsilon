#ifndef POINCARE_TREE_POOL_H
#define POINCARE_TREE_POOL_H

#include "tree_block.h"

namespace Poincare {

class TreePool {
public:
  TreeBlock * blockAtIndex(int i) { return firstBlock() + sizeof(TreeBlock) * i; }

  virtual TreeBlock * firstBlock() = 0;
  virtual TreeBlock * lastBlock() = 0;
};

}

#endif

