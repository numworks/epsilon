#include "pair_node.h"

namespace Poincare {

TreeNode * PairNode::uninitializedStaticNode() const {
  return UninitializedPairNode::UninitializedPairStaticNode();
}

PairNode * PairNode::FailedAllocationStaticNode() {
  static AllocationFailurePairNode failureNode;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failureNode);
  return &failureNode;
}

UninitializedPairNode * UninitializedPairNode::UninitializedPairStaticNode() {
  static UninitializedPairNode exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
