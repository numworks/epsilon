#include "tree_pool.h"

namespace Poincare {

#if POINCARE_TREE_LOG
void TreePool::flatLog(std::ostream & stream) {
  stream << "<TreeSandbox format=\"flat\" size=\"" << fullSize()  << "\">";
  for (TypeTreeBlock * block : allNodes()) {
    block->log(stream, false);
  }
  stream << "</TreePool>";
  stream << std::endl;
}

void TreePool::treeLog(std::ostream & stream, bool verbose) {
  stream << "<TreePool format=\"tree\" size=\"" << fullSize() << "\">";
  for (TypeTreeBlock * tree : trees()) {
    tree->log(stream, true, 1, verbose);
  }
  stream << std::endl;
  stream << "</TreePool>";
  stream << std::endl;
}

#endif

TreePool::Nodes TreePool::allNodes() {
  if (firstBlock() == nullptr) {
    return Nodes(nullptr, 0);
  }
  return Nodes(firstBlock(), lastBlock() - static_cast<TreeBlock *>(firstBlock()));
}

TreePool::Trees TreePool::trees() {
  if (firstBlock() == nullptr) {
    return Trees(nullptr, 0);
  }
  return Trees(firstBlock(), lastBlock() - static_cast<TreeBlock *>(firstBlock()));
}

}
