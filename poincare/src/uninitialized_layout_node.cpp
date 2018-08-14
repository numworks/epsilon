#include <poincare/uninitialized_layout_node.h>

namespace Poincare {

UninitializedLayoutNode * UninitializedLayoutNode::UninitializedLayoutStaticNode() {
  static UninitializedLayoutNode exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
