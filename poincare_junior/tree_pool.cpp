#include "tree_pool.h"

namespace Poincare {

#if POINCARE_TREE_LOG
void TreePool::flatLog(std::ostream & stream) {
  stream << "<TreeSandbox format=\"flat\" size=\"" << lastBlock() - static_cast<TreeBlock *>(firstBlock()) << "\">";
  for (TypeTreeBlock * block : allNodes()) {
    block->log(stream, false);
  }
  stream << "</TreePool>";
  stream << std::endl;
}

void TreePool::treeLog(std::ostream & stream, bool verbose) {
  stream << "<TreePool format=\"tree\" size=\"" << lastBlock() - static_cast<TreeBlock *>(firstBlock()) << "\">";
  for (TypeTreeBlock * tree : trees()) {
    tree->log(stream, true, 1, verbose);
  }
  stream << std::endl;
  stream << "</TreePool>";
  stream << std::endl;
}

#endif

}
