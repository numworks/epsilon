#include "tree_pool.h"

namespace Poincare {

TreeBlock * TreePool::previousBlock(TreeBlock * b) {
  if (b == firstBlock()) {
    return nullptr;
  }
  return b - sizeof(TreeBlock);
}

TreeBlock * TreePool::nextTree(TreeBlock * b) {
  int nbOfSubtreesToScan = b->numberOfSubtrees();
  TreeBlock * result = b;
  while (nbOfSubtreesToScan > 0) {
    result = nextBlock(result);
    nbOfSubtreesToScan += result->numberOfSubtrees() - 1;
  }
  return nextBlock(result);
}

}
