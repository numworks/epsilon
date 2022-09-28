#ifndef POINCARE_GHOST_INTERFACE_H
#define POINCARE_GHOST_INTERFACE_H

#include "interface.h"

namespace Poincare {

#warning remove ghost

#if GHOST_REQUIRED
class GhostInterface final : public Interface {
public:
   static constexpr size_t CreateNodeAtAddress(Block * address) {
    *(address) = GhostBlock;
    return k_numberOfBlocksInNode;
  }
  static TypeBlock * PushNode() { return Interface::PushNode<GhostInterface, k_numberOfBlocksInNode>(); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Ghost"; }
#endif

  static constexpr size_t k_numberOfBlocksInNode = 1;
};
#endif

}

#endif


