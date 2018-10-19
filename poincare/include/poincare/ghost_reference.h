#ifndef POINCARE_GHOST_REFERENCE_H
#define POINCARE_GHOST_REFERENCE_H

#include <poincare/ghost_node.h>
#include <poincare/tree_handle.h>
#include <poincare/tree_pool.h>

namespace Poincare {

/* GhostReference is not in ghost_node.h because GhostNode is needed in
 * tree_pool.h and this created header inclusion problems. */

class GhostReference final : public TreeHandle {
public:
  GhostReference() : TreeHandle(TreePool::sharedPool()->createTreeNode<GhostNode>()) {}
};

}

#endif
