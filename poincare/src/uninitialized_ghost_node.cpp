#include <poincare/uninitialized_ghost_node.h>

namespace Poincare {

UninitializedGhostNode * UninitializedGhostNode::UninitializedGhostStaticNode() {
  static UninitializedGhostNode exception;
  return &exception;
}

}
