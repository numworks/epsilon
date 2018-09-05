#include "pair_node.h"

namespace Poincare {

TreeNode * PairNode::uninitializedStaticNode() const {
  return UninitializedPairNode::UninitializedPairStaticNode();
}

UninitializedPairNode * UninitializedPairNode::UninitializedPairStaticNode() {
  static UninitializedPairNode exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
