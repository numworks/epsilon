#include <poincare/init.h>
#include <poincare/tree_pool.h>
#include <poincare/uninitialized_expression_node.h>
#include <poincare/uninitialized_ghost_node.h>

namespace Poincare {

void init() {
  // Create and register the shared static pool
  static TreePool pool;
  TreePool::RegisterPool(&pool);

  // Register static nodes
  pool.registerStaticNode(UninitializedExpressionNode::UninitializedExpressionStaticNode());
  pool.registerStaticNode(UninitializedGhostNode::UninitializedGhostStaticNode());
}

}
