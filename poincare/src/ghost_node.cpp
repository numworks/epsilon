#include <poincare/ghost_node.h>
#include <poincare/uninitialized_ghost_node.h>
#include <poincare/tree_pool.h>

namespace Poincare {

TreeNode * GhostNode::uninitializedStaticNode() const {
  return UninitializedGhostNode::UninitializedGhostStaticNode();
}

}
