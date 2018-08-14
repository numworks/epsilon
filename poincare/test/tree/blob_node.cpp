#include "blob_node.h"

namespace Poincare {

TreeNode * BlobNode::uninitializedStaticNode() const {
  return UninitializedBlobNode::UninitializedBlobStaticNode();
}

BlobNode * BlobNode::FailedAllocationStaticNode() {
  static AllocationFailureBlobNode failureNode;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&failureNode);
  return &failureNode;
}

UninitializedBlobNode * UninitializedBlobNode::UninitializedBlobStaticNode() {
  static UninitializedBlobNode exception;
  TreePool::sharedPool()->registerStaticNodeIfRequired(&exception);
  return &exception;
}

}
