#ifndef POINCARE_GHOST_INTERFACE_H
#define POINCARE_GHOST_INTERFACE_H

#include "interface.h"

namespace Poincare {

#warning remove ghost

#if GHOST_REQUIRED
class GhostInterface final : public Interface {
public:
  constexpr static bool CreateBlockAtIndex(Block * block, size_t blockIndex) {
    assert(blockIndex == 0);
    *block = GhostBlock;
    return true;
  }
  static TypeBlock * PushNode() { return Interface::PushNode<GhostInterface>(); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Ghost"; }
#endif

  static constexpr size_t k_numberOfBlocksInNode = 1;
};
#endif

}

#endif


